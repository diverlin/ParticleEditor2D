//
// Copyright (c) 2014 the ParticleEditor2D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#pragma once

#include <QWidget>

class QSpacerItem;
class QPushButton;

namespace Urho3D
{

class NodeItemWidget;

class NodeManagerWidget : public QWidget
{
    Q_OBJECT
public:
    NodeManagerWidget(QWidget* parent);
    virtual ~NodeManagerWidget();

    NodeItemWidget* itemWidget(const QString&) const;

    QList<QString> getDirtyKeys() const;

    void unmarkDirty(QString key);
    void markDirty(QString key);
    void add(NodeItemWidget*);
    bool remove(const QString&);
    bool changeKey(const QString&, const QString&);

signals:
    void visibleChanged(const QString&, bool);
    void deleteRequested(QString);
    void nodePositionChanged(const QString&, int, int);
    void acceptKeyChangeRequest(QString, QString);
    void selected(const QString&);
    void saveAllRequested();
    void saveRequested(const QString&);

private:
    QPushButton* m_pbToggleGrid = nullptr;
    QPushButton* m_pbSaveAll = nullptr;
    QSpacerItem* m_horizontalSpacer = nullptr;
    std::map<QString, NodeItemWidget*> m_widgets;

    NodeItemWidget* takeItemWidget(const QString&);
    bool isKeyUnique(const QString& key) const;
};

}
