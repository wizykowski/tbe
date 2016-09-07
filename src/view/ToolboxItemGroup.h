/* The Butterfly Effect
 * This file copyright (C) 2016 Klaas van Gend
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation
 * applicable version is GPL version 2 only.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA.
 */

#ifndef TOOLBOXITEMGROUP_H
#define TOOLBOXITEMGROUP_H

#include <QObject>

/// This class manages one group of identical items in the toolbox.
/// This class is responible for keeping refs to (maybe again) unused
/// AbstractObjects and/or ViewItems.
class ToolboxItemGroup : public QObject
{
    Q_OBJECT
public:
    explicit ToolboxItemGroup(QObject *parent = nullptr);
    ToolboxItemGroup(const QString &aName, int aCount, qreal aWidth, qreal aHeight,
                     const QString& anIconName, const QString &aTooltip,
                     QObject *parent = 0);

    Q_PROPERTY(QString name    MEMBER theName NOTIFY nameChanged)
    Q_PROPERTY(int     count   READ count WRITE setCount NOTIFY countChanged)
    Q_PROPERTY(qreal   owidth  MEMBER theWidth NOTIFY owidthChanged)
    Q_PROPERTY(qreal   oheight MEMBER theHeight NOTIFY oheightChanged)
    Q_PROPERTY(QString iconName MEMBER theIconName NOTIFY iconNameChanged)
    Q_PROPERTY(QString tooltip MEMBER theTooltipText NOTIFY tooltipChanged)

    QString name();
    int count() { return theCount; }
    qreal owidth();
    qreal oheight();
    QString iconName();
    QString tooltip();

    void setCount(int aNewCount)
    {theCount = aNewCount; emit countChanged();}

signals:
    void nameChanged();
    void countChanged();
    void owidthChanged();
    void oheightChanged();
    void iconNameChanged();
    void tooltipChanged();

public slots:

private:
    QString theName;
    int     theCount;
    qreal   theWidth;
    qreal   theHeight;
    QString theIconName;
    QString theTooltipText;
};

#endif // TOOLBOXITEMGROUP_H