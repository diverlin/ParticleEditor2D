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
#include <QDebug>

namespace Urho3D
{
    
NodeItemWidget::NodeItemWidget(const QString& key)
    :
    QWidget()
  , m_cbVisible(new QCheckBox(this))
  , m_leName(new QLineEdit(this))
  , m_pbDelete(new QPushButton(this))
  , m_leNodePosition(new QLineEdit(this))
  , m_key(key)
{
    auto layout = new QVBoxLayout;

    layout->addWidget(m_cbVisible);
    layout->addWidget(m_leName);
    layout->addWidget(m_pbDelete);
    layout->addWidget(m_leNodePosition);

    setLayout(layout);

    m_leName->setText(key);
    m_leName->setReadOnly(true);
    m_cbVisible->setText("eye");
    m_pbDelete->setText("del");
    m_leNodePosition->setText("0,0");

    connect(m_pbDelete, &QPushButton::clicked, [this]() {
        emit deleteRequested(m_key);
    });
    connect(m_cbVisible, &QPushButton::toggled, [this](bool checked) {
        emit visibleChanged(m_key, checked);
    });

    connect(m_leNodePosition, &QLineEdit::textChanged, this, [this](const QString& data) {
        int x, y = 0;
        QStringList numbers = data.split(",");
        if (numbers.size() == 2) {
            x = std::atoi(numbers[0].toStdString().c_str());
            y = std::atoi(numbers[1].toStdString().c_str());
            emit nodePositionChanged(m_key, x, y);
        }
    });

}

NodeItemWidget::~NodeItemWidget()
{
}

}
