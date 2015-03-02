/*
 * Copyright (c) 2011 Eric Brandt (eric.l.brandt@gmail.com)
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "Context.h"
#include "Athlete.h"
#include "RideFile.h"
#include "RideItem.h"
#include "RideMetric.h"
#include "IntervalItem.h"
#include "IntervalTreeView.h"
#include "IntervalSummaryWindow.h"
#include "Settings.h"
#include "TimeUtils.h"
#include "Colors.h"
#include <QStyle>
#include <QStyleFactory>
#include <QScrollBar>

IntervalSummaryWindow::IntervalSummaryWindow(Context *context) : context(context)
{
    setWindowTitle(tr("Interval Summary"));
    setReadOnly(true);
    setFrameStyle(QFrame::NoFrame);
#ifdef Q_OS_WIN
    QStyle *cde = QStyleFactory::create(OS_STYLE);
    verticalScrollBar()->setStyle(cde);
#endif

#ifdef Q_OS_MAC
    setAttribute(Qt::WA_MacShowFocusRect, 0);
#endif
    connect(context, SIGNAL(intervalsChanged()), this, SLOT(intervalSelected()));
    connect(context, SIGNAL(intervalSelected()), this, SLOT(intervalSelected()));
    connect(context, SIGNAL(intervalHover(RideFileInterval)), this, SLOT(intervalHover(RideFileInterval)));
    connect(context, SIGNAL(configChanged(qint32)), this, SLOT(intervalSelected()));

    setHtml(GCColor::css() + "<body></body>");
}

IntervalSummaryWindow::~IntervalSummaryWindow() {
}

void IntervalSummaryWindow::intervalSelected()
{
    // if no ride available don't bother - just reset for color changes
    RideItem *rideItem = const_cast<RideItem*>(context->currentRideItem());

    if (context->athlete->intervalTreeWidget()->selectedItems().count() == 0 || 
        rideItem == NULL || rideItem->ride() == NULL) {
        // no ride just update the colors
	    QString html = GCColor::css();
        html += "<body></body>";
	    setHtml(html);
	    return;
    }

    // summary is html
	QString html = GCColor::css();
    html += "<body>";

    // summary of all intervals selected
    QList<IntervalItem*> list;
    if (context->athlete->allIntervalItems() != NULL) {
        for (int i=0; i<context->athlete->allIntervalItems()->childCount(); i++) {
            IntervalItem *current = dynamic_cast<IntervalItem*>(context->athlete->allIntervalItems()->child(i));
            if (current != NULL) {
                if (current->isSelected()) {
                    list << current;
                }
            }
        }
    }
    if (list.count()>1) calcInterval(list, html);

    // summary for each of the currently selected intervals
    if (context->athlete->allIntervalItems() != NULL) {
        for (int i=0; i<context->athlete->allIntervalItems()->childCount(); i++) {
            IntervalItem *current = dynamic_cast<IntervalItem*>(context->athlete->allIntervalItems()->child(i));
            if (current != NULL) {
                if (current->isSelected()) {
                    list << current;
                    calcInterval(current, html);
                }
            }
        }
    }

    if (html == GCColor::css()+"<body>") {
    	html += "<i>" + tr("select an interval for summary info") + "</i>";
    }

    html += "</body>";
	setHtml(html);
	return;
}

void
IntervalSummaryWindow::intervalHover(RideFileInterval x)
{
    // if we're not visible don't bother
    if (!isVisible()) return;

    // we already have summaries!
    if (context->athlete->intervalWidget->selectedItems().count()) return;

    QString html = GCColor::css();
    html += "<body>";

    if (x == RideFileInterval()) {
    	html += "<i>" + tr("select an interval for summary info") + "</i>";
    } else {
        calcInterval(x, html);
    }
    html += "</body>";
    setHtml(html);
    return;
}

static bool contains(const RideFile*ride, QList<IntervalItem*> intervals, int index)
{
    foreach(IntervalItem *item, intervals) {
        int start = ride->timeIndex(item->start);
        int end = ride->timeIndex(item->stop);

        if (index >= start && index <= end) return true;
    }
    return false;
}

void IntervalSummaryWindow::calcInterval(QList<IntervalItem*> intervals, QString &html)
{
	const RideFile* ride = context->ride ? context->ride->ride() : NULL;

    RideFile f(const_cast<RideFile*>(ride));

    // for concatenating intervals
    RideFilePoint *last = NULL;
    double timeOff=0;
    double distOff=0;

    for (int i = 0; i < ride->dataPoints().count(); ++i) {

        // append points for selected intervals
        const RideFilePoint *p = ride->dataPoints()[i];
        if (contains(ride, intervals, i)) {

            f.appendPoint(p->secs-timeOff, p->cad, p->hr, p->km-distOff, p->kph, p->nm,
                        p->watts, p->alt, p->lon, p->lat, p->headwind, p->slope, p->temp, p->lrbalance, 
                        p->lte, p->rte, p->lps, p->rps,
                        p->lpco, p->rpco,
                        p->lppb, p->rppb, p->lppe, p->rppe,
                        p->lpppb, p->rpppb, p->lpppe, p->rpppe,
                        p->smo2, p->thb,
                        p->rvert, p->rcad, p->rcontact, 0);

            // derived data
            last = f.dataPoints().last();
            last->np = p->np;
            last->xp = p->xp;
            last->apower = p->apower;

        } else {

            if (last) {
                timeOff = p->secs - last->secs;
                distOff = p->km - last->km;
            } else {
                timeOff = p->secs;
                distOff = p->km;
            }
        }
    }

    summary(f, QString("%1 intervals").arg(intervals.count()), html);
}

void IntervalSummaryWindow::calcInterval(IntervalItem* interval, QString& html)
{
	const RideFile* ride = context->ride ? context->ride->ride() : NULL;

    RideFile f(const_cast<RideFile*>(ride));
    int start = ride->timeIndex(interval->start);
    int end = ride->timeIndex(interval->stop);
    for (int i = start; i <= end; ++i) {
        const RideFilePoint *p = ride->dataPoints()[i];
        f.appendPoint(p->secs, p->cad, p->hr, p->km, p->kph, p->nm,
                      p->watts, p->alt, p->lon, p->lat, p->headwind, p->slope, p->temp, p->lrbalance, 
                      p->lte, p->rte, p->lps, p->rps,
                      p->lpco, p->rpco,
                      p->lppb, p->rppb, p->lppe, p->rppe,
                      p->lpppb, p->rpppb, p->lpppe, p->rpppe,
                      p->smo2, p->thb,
                      p->rvert, p->rcad, p->rcontact, 0);

        // derived data
        RideFilePoint *l = f.dataPoints().last();
        l->np = p->np;
        l->xp = p->xp;
        l->apower = p->apower;
    }

    summary(f, interval->text(0), html);
}

void IntervalSummaryWindow::summary(RideFile &f, QString name, QString &html)
{
    bool metricUnits = context->athlete->useMetricUnits;

    if (f.dataPoints().size() == 0) {
        // Interval empty, do not compute any metrics
        html += "<i>" + tr("empty interval") + "</tr>";
    }

    QString s;
    if (appsettings->contains(GC_SETTINGS_INTERVAL_METRICS))
        s = appsettings->value(this, GC_SETTINGS_INTERVAL_METRICS).toString();
    else
        s = GC_SETTINGS_INTERVAL_METRICS_DEFAULT;
    QStringList intervalMetrics = s.split(",");

    QHash<QString,RideMetricPtr> metrics =
        RideMetric::computeMetrics(context, &f, context->athlete->zones(), context->athlete->hrZones(), intervalMetrics);

    html += "<b>" + name + "</b>";
    html += "<table align=\"center\" width=\"90%\" ";
    html += "cellspacing=0 border=0>";

    RideItem *rideItem = const_cast<RideItem*>(context->currentRideItem());

    foreach (QString symbol, intervalMetrics) {
        RideMetricPtr m = metrics.value(symbol);
        if (!m) continue;

        // skip metrics that are not relevant for this ride
        if (!rideItem || m->isRelevantForRide(rideItem) == false) continue;

        html += "<tr>";
        // left column (names)
        html += "<td align=\"right\" valign=\"bottom\">" + m->name() + "</td>";

        // right column (values)
        QString s("<td align=\"center\">%1</td>");
        html += s.arg(m->toString(metricUnits));
        html += "<td align=\"left\" valign=\"bottom\">";
        if (m->units(metricUnits) == "seconds" ||
            m->units(metricUnits) == tr("seconds"))
            ; // don't do anything
        else if (m->units(metricUnits).size() > 0)
            html += m->units(metricUnits);
        html += "</td>";

        html += "</tr>";

    }
    html += "</table>";
}

void IntervalSummaryWindow::calcInterval(RideFileInterval interval, QString& html)
{
	const RideFile* ride = context->ride ? context->ride->ride() : NULL;

    bool metricUnits = context->athlete->useMetricUnits;

    RideFile f(const_cast<RideFile*>(ride));
    int start = ride->timeIndex(interval.start);
    int end = ride->timeIndex(interval.stop);
    for (int i = start; i <= end; ++i) {
        const RideFilePoint *p = ride->dataPoints()[i];
        f.appendPoint(p->secs, p->cad, p->hr, p->km, p->kph, p->nm,
                      p->watts, p->alt, p->lon, p->lat, p->headwind, p->slope, p->temp, p->lrbalance, 
                      p->lte, p->rte, p->lps, p->rps,
                      p->lpco, p->rpco,
                      p->lppb, p->rppb, p->lppe, p->rppe,
                      p->lpppb, p->rpppb, p->lpppe, p->rpppe,
                      p->smo2, p->thb,
                      p->rvert, p->rcad, p->rcontact, 0);

        // derived data
        RideFilePoint *l = f.dataPoints().last();
        l->np = p->np;
        l->xp = p->xp;
        l->apower = p->apower;
    }
    if (f.dataPoints().size() == 0) {
        // Interval empty, do not compute any metrics
        html += "<i>" + tr("empty interval") + "</tr>";
    }

    QString s;
    if (appsettings->contains(GC_SETTINGS_INTERVAL_METRICS))
        s = appsettings->value(this, GC_SETTINGS_INTERVAL_METRICS).toString();
    else
        s = GC_SETTINGS_INTERVAL_METRICS_DEFAULT;
    QStringList intervalMetrics = s.split(",");

    QHash<QString,RideMetricPtr> metrics =
        RideMetric::computeMetrics(context, &f, context->athlete->zones(), context->athlete->hrZones(), intervalMetrics);

    html += "<b>" + interval.name + "</b>";
    html += "<table align=\"center\" width=\"90%\" ";
    html += "cellspacing=0 border=0>";

    RideItem *rideItem = const_cast<RideItem*>(context->currentRideItem());

    foreach (QString symbol, intervalMetrics) {
        RideMetricPtr m = metrics.value(symbol);
        if (!m) continue;

        // skip metrics that are not relevant for this ride
        if (!rideItem || m->isRelevantForRide(rideItem) == false) continue;


        html += "<tr>";
        // left column (names)
        html += "<td align=\"right\" valign=\"bottom\">" + m->name() + "</td>";

        // right column (values)
        QString s("<td align=\"center\">%1</td>");
        html += s.arg(m->toString(metricUnits));
        html += "<td align=\"left\" valign=\"bottom\">";
        if (m->units(metricUnits) == "seconds" ||
            m->units(metricUnits) == tr("seconds"))
            ; // don't do anything
        else if (m->units(metricUnits).size() > 0)
            html += m->units(metricUnits);
        html += "</td>";

        html += "</tr>";

    }
    html += "</table>";
}
