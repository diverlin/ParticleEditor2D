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

namespace Urho3D
{
    
NodeManagerWidget::NodeManagerWidget(QWidget* parent) :
    QWidget(parent)
{
    QHBoxLayout* layout = new QHBoxLayout;
    setLayout(layout);
}

NodeManagerWidget::~NodeManagerWidget()
{
}

void NodeManagerWidget::add(NodeItemWidget* item)
{
    auto it = m_widgets.find(item->key());
    assert(it == m_widgets.end());

    connect(item, &NodeItemWidget::visibleChanged, this, [this](const QString& key, bool visible){
        emit visibleChanged(key, visible);
    });
    connect(item, &NodeItemWidget::deleteRequested, this, [this](const QString& key){
        emit deleteRequested(key);
    });
    connect(item, &NodeItemWidget::nodePositionChanged, this, [this](const QString& key, int x, int y){
        emit nodePositionChanged(key, x, y);
    });

    m_widgets.insert(std::make_pair(item->key(), item));
    layout()->addWidget(item);
}

bool NodeManagerWidget::remove(const QString& key)
{
    auto it = m_widgets.find(key);
    if (it != m_widgets.end()) {
        NodeItemWidget* item = it->second;
        layout()->removeWidget(item);
        delete item;
        return true;
    }
    return false;
}

}
