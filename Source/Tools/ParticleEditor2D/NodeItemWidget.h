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

class QPushButton;
class QLineEdit;
class QCheckBox;

namespace Urho3D
{

class NodeItemWidget : public QWidget
{
    Q_OBJECT
public:
    NodeItemWidget(QWidget* parent, const QString& key);
    virtual ~NodeItemWidget();

    void setNodePosition(int x, int y);
    void setKey(const QString& key) { m_key = key; }

    const QString& key() const { return m_key; }
    void rejectNewKeyCandidate();
    void acceptNewKeyCandidate();

    void select();
    void deselect();

signals:
    void selected(const QString&);
    void visibleChanged(const QString&, bool);
    void deleteRequested(QString);
    void nodePositionChanged(const QString&, int x, int y);
    void changeKeyRequest(QString, QString);
    void saveRequested(const QString&);

protected:
    void mousePressEvent(QMouseEvent*) override final;

private:
    bool m_isSelected = false;
    QString m_key;
    QCheckBox* m_cbVisible = nullptr;
    QPushButton* m_pbSelect = nullptr;
    QPushButton* m_pbSave = nullptr;
    QPushButton* m_pbClone = nullptr;
    QPushButton* m_pbDelete = nullptr;
    QLineEdit* m_leName = nullptr;
    QLineEdit* m_leNodePosition = nullptr;
};

} // namespace Urho3D
