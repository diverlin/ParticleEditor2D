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

#include "NodeItemWidget.h"

#include <QVBoxLayout>

#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>

namespace Urho3D
{
    
NodeItemWidget::NodeItemWidget(const QString& key)
    :
    QWidget()
  , m_checkboxVisible(new QCheckBox(this))
  , m_lineeditName(new QLineEdit(this))
  , m_pushbuttonDelete(new QPushButton(this))
  , m_key(key)
{
    auto layout = new QVBoxLayout;

    layout->addWidget(m_checkboxVisible);
    layout->addWidget(m_lineeditName);
    layout->addWidget(m_pushbuttonDelete);

    setLayout(layout);

    m_lineeditName->setText(key);
    m_lineeditName->setReadOnly(true);
    m_checkboxVisible->setText("eye");
    m_pushbuttonDelete->setText("del");

    connect(m_pushbuttonDelete, &QPushButton::clicked, [this]() {
        emit deleteRequested(m_key);
    });
    connect(m_checkboxVisible, &QPushButton::toggled, [this](bool checked) {
        emit visibleChanged(m_key, checked);
    });
}

NodeItemWidget::~NodeItemWidget()
{
}

}
