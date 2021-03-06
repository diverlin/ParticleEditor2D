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

#include "NodeManagerWidget.h"
#include "NodeItemWidget.h"

#include <QHBoxLayout>
#include <QSpacerItem>
#include <QPushButton>
#include <QDebug>

namespace Urho3D
{
    
NodeManagerWidget::NodeManagerWidget(QWidget* parent) :
    QWidget(parent)
  , m_pbToggleGrid(new QPushButton(tr("grid"), this))
  , m_pbSaveAll(new QPushButton(tr("save all"), this))
  , m_horizontalSpacer(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum))
{
    setLayout(new QHBoxLayout);

    layout()->setSizeConstraint(QLayout::SetMinimumSize);
    layout()->setContentsMargins(0,0,0,0);
    layout()->setSpacing(0);

    QWidget* vBar = new QWidget(this);
    vBar->setLayout(new QVBoxLayout);
    vBar->layout()->addWidget(m_pbToggleGrid);
    vBar->layout()->addWidget(m_pbSaveAll);
    vBar->layout()->addItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding));

    layout()->addWidget(vBar);
    layout()->addItem(m_horizontalSpacer);

    connect(m_pbToggleGrid, &QPushButton::clicked, this, [this]() {
        float step = 2;
        int size = m_widgets.size();
        int rows = 2;
        float size_x = float(size)/rows;

        float left_border_x = -size/size_x * step;
        float right_border_x = size/size_x * step;

        float x = left_border_x;
        float y = 0;
        auto it = m_widgets.begin();
        for(; it!=m_widgets.end(); ++it) {
            NodeItemWidget* widget = it->second;
            widget->setNodePosition(x,y);
            x += step;
            if (x >= right_border_x) {
                x = left_border_x;
                y += step;
            }
        }
    });

    connect(m_pbSaveAll, &QPushButton::clicked, this, [this]{
        emit saveAllRequested();
    });
}

NodeManagerWidget::~NodeManagerWidget()
{
}

void NodeManagerWidget::add(NodeItemWidget* item)
{
    auto it = m_widgets.find(item->key());
    assert(it == m_widgets.end());

    connect(item, &NodeItemWidget::visibleChanged, this, &NodeManagerWidget::visibleChanged);
    connect(item, &NodeItemWidget::selected, this, [this](const QString& key){
        emit selected(key);
        for (auto it: m_widgets) {
            auto w = it.second;
            if (w->key() == key) {
                w->select();
            } else {
                w->deselect();
            }
        }
    });
    connect(item, &NodeItemWidget::restartEmiterRequest, this, &NodeManagerWidget::restartEmiterRequest);
    connect(item, &NodeItemWidget::saveRequested, this, &NodeManagerWidget::saveRequested);
    connect(item, &NodeItemWidget::deleteRequested, this, [this](QString key){
        assert(remove(key));
        emit deleteRequested(key);
    });
    connect(item, &NodeItemWidget::nodePositionChanged, this, &NodeManagerWidget::nodePositionChanged);
    connect(item, &NodeItemWidget::changeKeyRequest, this, [this](QString key, QString newKeyCandidate){
        NodeItemWidget* widget = itemWidget(key);
        if (isKeyUnique(newKeyCandidate)) {
            assert(changeKey(key, newKeyCandidate));
            widget->acceptNewKeyCandidate();
            emit acceptKeyChangeRequest(key, newKeyCandidate);
        } else {
            widget->rejectNewKeyCandidate();
        }
    });

    m_widgets.insert(std::make_pair(item->key(), item));
    layout()->removeItem(m_horizontalSpacer);
    layout()->addWidget(item);
    layout()->addItem(m_horizontalSpacer);

    if (m_widgets.size() == 1) {
        item->emitSelected();
    }
}

bool NodeManagerWidget::hasUnsaved() const {
    return (getDirtyKeys().size() !=0);
}

QList<QString> NodeManagerWidget::getDirtyKeys() const {
    QList<QString> keys;
    for(auto it: m_widgets) {
        if (it.second->isDirty()) {
            keys << it.first;
        }
    }
    return keys;
}

void NodeManagerWidget::markDirty(QString key) {
    NodeItemWidget* widget = itemWidget(key);
    if (widget) {
        widget->markDirty();
    }
}

void NodeManagerWidget::unmarkDirty(QString key) {
    NodeItemWidget* widget = itemWidget(key);
    if (widget) {
        widget->unmarkDirty();
    }
}

NodeItemWidget* NodeManagerWidget::itemWidget(const QString& key) const {
    auto it = m_widgets.find(key);
    if (it != m_widgets.end()) {
        NodeItemWidget* item = it->second;
        return item;
    }
    return nullptr;
}

NodeItemWidget* NodeManagerWidget::takeItemWidget(const QString& key) {
    auto it = m_widgets.find(key);
    if (it != m_widgets.end()) {
        NodeItemWidget* item = it->second;
        m_widgets.erase(it);
        return item;
    }
    return nullptr;
}

bool NodeManagerWidget::changeKey(const QString& fromKey, const QString& toKey)
{
    NodeItemWidget* widget = takeItemWidget(fromKey);
    if (widget) {
        widget->setKey(toKey);
        m_widgets.insert(std::make_pair(toKey, widget));
        return true;
    }
    return false;
}

bool NodeManagerWidget::remove(const QString& key)
{
    NodeItemWidget* widget = takeItemWidget(key);
    if (widget) {
        layout()->removeWidget(widget);
        delete widget;
        return true;
    } else {
        return false;
    }
}

bool NodeManagerWidget::isKeyUnique(const QString& key) const
{
    NodeItemWidget* widget = itemWidget(key);
    return (widget == nullptr);
}

} // namespace Urho3D
