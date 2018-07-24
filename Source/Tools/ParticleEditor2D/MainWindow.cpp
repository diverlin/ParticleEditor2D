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

#include "EmitterAttributeEditor.h"
#include "MainWindow.h"
#include "ParticleAttributeEditor.h"
#include "ParticleEditor.h"
#include "NodeManagerWidget.h"
#include "NodeItemWidget.h"
#include "PathUtils.h"

#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/Zone.h>

#include <QSettings>
#include <QAction>
#include <QColorDialog>
#include <QDockWidget>
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QDebug>
#include <QResizeEvent>
#include <QMessageBox>

namespace {
const QString LAST_PATH("lastPath");
const QString LAST_PS("lastPs");
}

namespace Urho3D
{

MainWindow::MainWindow(Context* context) :
    QMainWindow(0, 0),
    ParticleEffectEditor(context),
    emitterAttributeEditor_(nullptr),
    particleAttributeEditor_(nullptr)
{
    setWindowIcon(QIcon(":/Images/Icon.png"));

    QWidget* widget = new QWidget();
    setCentralWidget(widget);
    
    widget->setFixedSize(1200, 700); // workaround, the SDL2_GetWindowSize(window, width, height) doesn't get actual window size when QWidget is actually resized
    widget->setUpdatesEnabled(false);
    widget->setFocusPolicy(Qt::StrongFocus);

    showMaximized();
}

void MainWindow::CreateWidgets()
{
    CreateActions();
    CreateMenuBar();
    CreateToolBar();
    CreateDockWidgets();
}

void MainWindow::HandleUpdateWidget()
{
    if (emitterAttributeEditor_)
        emitterAttributeEditor_->UpdateWidget();
    if (particleAttributeEditor_)
        particleAttributeEditor_->UpdateWidget();
}

void MainWindow::CreateActions()
{
    newAction_ = new QAction(QIcon(":/Images/New.png"), tr("New"), this);
    newAction_->setShortcut(QKeySequence::fromString("Ctrl+N"));
    connect(newAction_, SIGNAL(triggered(bool)), this, SLOT(HandleNewAction()));

    openAction_ = new QAction(QIcon(":/Images/Open.png"), tr("Open ..."), this);
    openAction_->setShortcut(QKeySequence::fromString("Ctrl+O"));
    connect(openAction_, SIGNAL(triggered(bool)), this, SLOT(HandleOpenAction()));

    saveAction_= new QAction(QIcon(":/Images/Save.png"), tr("Save ..."), this);
    saveAction_->setShortcut(QKeySequence::fromString("Ctrl+S"));
    connect(saveAction_, SIGNAL(triggered(bool)), this, SLOT(HandleSaveAction()));

    saveAsAction_ = new QAction(QIcon(":/Images/SaveAs.png"), tr("Save As ..."), this);
    saveAsAction_->setShortcut(QKeySequence::fromString("Ctrl+Shift+S"));
    connect(saveAsAction_, SIGNAL(triggered(bool)), this, SLOT(HandleSaveAsAction()));

    exitAction_ = new QAction(tr("Exit"), this);
    exitAction_->setShortcut(QKeySequence::fromString("Alt+F4"));
    connect(exitAction_, &QAction::triggered, this, [this](bool){
        if (CheckClosePermition()) {
            close();
        }
    });

    zoomInAction_ = new QAction(QIcon(":/Images/ZoomIn.png"), tr("Zoom In"), this);
    zoomInAction_->setShortcut(QKeySequence::fromString("Ctrl++"));
    connect(zoomInAction_, SIGNAL(triggered(bool)), this, SLOT(HandleZoomAction()));

    zoomOutAction_ = new QAction(QIcon(":/Images/ZoomOut.png"), tr("Zoom Out"), this);
    zoomOutAction_->setShortcut(QKeySequence::fromString("Ctrl+-"));
    connect(zoomOutAction_, SIGNAL(triggered(bool)), this, SLOT(HandleZoomAction()));

    zoomResetAction_ = new QAction(QIcon(":/Images/ZoomReset.png"), tr("Zoom Reset"), this);
    zoomResetAction_->setShortcut(QKeySequence::fromString("Ctrl+0"));
    connect(zoomResetAction_, SIGNAL(triggered(bool)), this, SLOT(HandleZoomAction()));

    backgroundAction_ = new QAction(tr("Background"), this);
    backgroundAction_->setShortcut(QKeySequence::fromString("Ctrl+B"));
    connect(backgroundAction_, SIGNAL(triggered(bool)), this, SLOT(HandleBackgroundAction()));
}

bool MainWindow::CheckClosePermition() const {
    if (nodeManagerWidget_->hasUnsaved()) {
        if (QMessageBox::Ok != showQuestionMessageBox("There are some unsaved configurations. Still want to exit and lost data?")) {
            return false;
        }
    }
    return true;
}

void MainWindow::closeEvent(QCloseEvent* event) {

    if (!CheckClosePermition()) {
        event->ignore();
    } else {
        event->accept();
    }
}

static QAction* CreateAction(QActionGroup* group, const QString& iconFileName, const QString& text, bool checked, const QString& shortcut = "")
{
    QAction* action = group->addAction(QIcon(iconFileName), text);
    action->setCheckable(true);
    action->setChecked(checked);
    if (!shortcut.isEmpty())
        action->setShortcut(QKeySequence::fromString(shortcut));
    return action;
}

void MainWindow::CreateMenuBar()
{
    fileMenu_ = menuBar()->addMenu(tr("&File"));

    fileMenu_->addAction(newAction_);
    fileMenu_->addAction(openAction_);
    fileMenu_->addAction(saveAction_);
    fileMenu_->addAction(saveAsAction_);

    fileMenu_->addSeparator();
    
    fileMenu_->addAction(exitAction_);

    viewMenu_ = menuBar()->addMenu(tr("&View"));

    viewMenu_->addAction(zoomInAction_);
    viewMenu_->addAction(zoomOutAction_);
    viewMenu_->addAction(zoomResetAction_);
    
    viewMenu_->addSeparator();

    viewMenu_->addAction(backgroundAction_);
}

void MainWindow::CreateToolBar()
{
    toolBar_ = addToolBar(tr("Tool Bar"));

    toolBar_->addAction(newAction_);
    toolBar_->addAction(openAction_);
    toolBar_->addAction(saveAction_);
    toolBar_->addAction(saveAsAction_);

    toolBar_->addSeparator();

    toolBar_->addAction(zoomInAction_);
    toolBar_->addAction(zoomOutAction_);
    toolBar_->addAction(zoomResetAction_);
}

void MainWindow::CreateDockWidgets()
{
    nodeManagerWidget_ = new NodeManagerWidget(this);
    QDockWidget* topDockWidget = new QDockWidget(tr("Layers"));
    addDockWidget(Qt::TopDockWidgetArea, topDockWidget);
    topDockWidget->setWidget(nodeManagerWidget_);

    connect(ParticleEditor::Get(), &ParticleEditor::NewParticleNodeAdded, this, [this](QString key) {
        NodeItemWidget* nodeItemWIdget = new NodeItemWidget(nodeManagerWidget_, key);
        nodeManagerWidget_->add(nodeItemWIdget);
    });

    connect(nodeManagerWidget_, &NodeManagerWidget::visibleChanged, this, [this](const QString& key, bool visible) {
        assert(ParticleEditor::Get()->SetVisible(String(key.toStdString().c_str()), visible));
    });
    connect(nodeManagerWidget_, &NodeManagerWidget::deleteRequested, this, [this](QString key) {
        assert(ParticleEditor::Get()->RemoveParticleNode(String(key.toStdString().c_str())));
        SaveOpenedPS();
    });
    connect(nodeManagerWidget_, &NodeManagerWidget::nodePositionChanged, this, [this](const QString& key, int x, int y) {
        assert(ParticleEditor::Get()->SetParticleNodePosition(String(key.toStdString().c_str()), x, y));
    });
    connect(nodeManagerWidget_, &NodeManagerWidget::acceptKeyChangeRequest, this, [this](QString key, QString newKey) {
        assert(ParticleEditor::Get()->changeKey(String(key.toStdString().c_str()), String(newKey.toStdString().c_str())));
    });
    connect(nodeManagerWidget_, &NodeManagerWidget::selected, this, [this](QString key) {
        String key_(key.toStdString().c_str());
        assert(ParticleEditor::Get()->select(key_));
        SetSelectedKey(key_);
        HandleUpdateWidget();
    });
    connect(nodeManagerWidget_, &NodeManagerWidget::saveAllRequested, this, [this]() {
        QList<QString> keys = nodeManagerWidget_->getDirtyKeys();
        for (QString key: keys) {
            if (ParticleEditor::Get()->Save(String(key.toStdString().c_str()))) {
                nodeManagerWidget_->unmarkDirty(key);
            }
        }
    });
    connect(nodeManagerWidget_, &NodeManagerWidget::saveRequested, this, [this](const QString& key) {
        if (ParticleEditor::Get()->Save(String(key.toStdString().c_str()))) {
            nodeManagerWidget_->unmarkDirty(key);
        }
    });

    emitterAttributeEditor_ = new EmitterAttributeEditor(context_);
    connect(emitterAttributeEditor_, &EmitterAttributeEditor::changed, this, [this](QString key) {
        nodeManagerWidget_->markDirty(key);
    });

    QDockWidget* eaDockWidget = new QDockWidget(tr("Emitter Attributes"));
    addDockWidget(Qt::LeftDockWidgetArea, eaDockWidget);
    eaDockWidget->setWidget(emitterAttributeEditor_);

    viewMenu_->addSeparator();
    
    QAction* eaToggleViewAction = eaDockWidget->toggleViewAction();
    viewMenu_->addAction(eaToggleViewAction);
    eaToggleViewAction->setShortcut(QKeySequence::fromString("Ctrl+E"));

    particleAttributeEditor_ = new ParticleAttributeEditor(context_);
    connect(particleAttributeEditor_, &ParticleAttributeEditor::changed, this, [this](QString key) {
        nodeManagerWidget_->markDirty(key);
    });

    QDockWidget* paDockWidget = new QDockWidget(tr("Particle Attributes"));
    addDockWidget(Qt::RightDockWidgetArea, paDockWidget);
    paDockWidget->setWidget(particleAttributeEditor_ );

    QAction* paToggleViewAction = paDockWidget->toggleViewAction();
    viewMenu_->addAction(paToggleViewAction);
    paToggleViewAction->setShortcut(QKeySequence::fromString("Ctrl+P"));
}

void MainWindow::SetSelectedKey(String key)
{
    ParticleEffectEditor::SetSelectedKey(key);
    if (emitterAttributeEditor_)
        emitterAttributeEditor_->SetSelectedKey(key);
    if (particleAttributeEditor_)
        particleAttributeEditor_->SetSelectedKey(key);
}

void MainWindow::HandleNewAction()
{
    showInfoMessageBox("Not implemented yet...");
}

void MainWindow::HandleOpenAction()
{
    QSettings settings;
    QString path = settings.value(LAST_PATH, "").toString();
    if (!QFileInfo(path).exists()) {
        path = QApplication::applicationDirPath() +"/Data/Urho2D";
    }

    QString filepath = QFileDialog::getOpenFileName(0, tr("Open particle"), path, "*.pex");
    if (filepath.isEmpty())
        return;

    path = QFileInfo(filepath).absolutePath();
    settings.setValue(LAST_PATH, path);

    bool result = ParticleEditor::Get()->Open(filepath.toLatin1().data());
    if (result) {
        SaveOpenedPS();
    }
}

void MainWindow::SaveOpenedPS() const
{
    QSettings settings;
    QList<QString> keys = ParticleEditor::Get()->GetKeys();
    QString data = keys.join(";");
    settings.setValue(LAST_PS, data);
}

void MainWindow::OpenPrevioslyOpenedPS() const
{
    QSettings settings;
    QString data = settings.value(LAST_PS, "").toString();
    QList<QString> keys = data.split(";");
    for (QString key: keys) {
        if (QFileInfo(key).exists()) {
            ParticleEditor::Get()->Open(key);
        }
    }
}

void MainWindow::HandleSaveAction()
{
    const String& fileName = ParticleEditor::Get()->GetFileName();
    if (fileName.Empty())
        HandleSaveAsAction();
    else
        ParticleEditor::Get()->Save(fileName);
}

void MainWindow::HandleSaveAsAction()
{
    QString fileName = QFileDialog::getSaveFileName(0, tr("Open particle"), "./Data/Urho2D/", "*.pex");
    if (fileName.isEmpty())
        return;

    ParticleEditor::Get()->Save(fileName.toLatin1().data());
}

void MainWindow::HandleZoomAction()
{
    Camera* camera = ParticleEditor::Get()->GetCamera();
    
    QObject* s = sender();
    if (s == zoomInAction_)
        camera->SetZoom(camera->GetZoom() * 1.25f);
    else if (s == zoomOutAction_)
        camera->SetZoom(camera->GetZoom() * 0.80f);
    else if (s == zoomResetAction_)
        camera->SetZoom(1.0f);
}

void MainWindow::HandleBackgroundAction()
{
    Renderer* renderer = GetSubsystem<Renderer>();
    const Color& color = renderer->GetDefaultZone()->GetFogColor();
    
    QColor qColor = QColor::fromRgbF(color.r_, color.g_, color.b_);
    QColor newQcolor = QColorDialog::getColor(qColor, this);
    
    Color newColor(newQcolor.redF(), newQcolor.greenF(), newQcolor.blueF());
    renderer->GetDefaultZone()->SetFogColor(newColor);
}

int showInfoMessageBox(const QString& msg)
{
    QMessageBox msgBox;
    msgBox.setText(msg);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    return msgBox.exec();
}

int showQuestionMessageBox(const QString& msg)
{
    QMessageBox msgBox;
    msgBox.setText(msg);
    msgBox.setStandardButtons(QMessageBox::Ok|QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    return msgBox.exec();
}

} // namespace Urho3D
