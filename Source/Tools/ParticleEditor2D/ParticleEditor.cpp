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

#include "ParticleEditor.h"
#include "MainWindow.h"

#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Engine/Console.h>
#include <Urho3D/Engine/EngineDefs.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Engine/DebugHud.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Urho2D/ParticleEffect2D.h>
#include <Urho3D/Urho2D/ParticleEmitter2D.h>
#include <Urho3D/Core/ProcessUtils.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/Resource/XMLFile.h>

#include <QFile>
#include <QTimer>
#include <QDebug>

namespace Urho3D
{

ParticleEditor::ParticleEditor(int argc, char** argv, Context* context) :
    QApplication(argc, argv),
    Object(context),
    engine_(new Engine(context_)),
    scene_(new Scene(context_)),
    mainWindow_(new MainWindow(context_))
{
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(ParticleEditor, HandleUpdate));
    SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(ParticleEditor, HandleKeyDown));
    SubscribeToEvent(E_MOUSEWHEEL, URHO3D_HANDLER(ParticleEditor, HandleMouseWheel));
    SubscribeToEvent(E_RENDERUPDATE, URHO3D_HANDLER(ParticleEditor, HandleRenderUpdate));
}

ParticleEditor::~ParticleEditor()
{
    delete mainWindow_;
}

int ParticleEditor::Run()
{
    VariantMap engineParameters;
    engineParameters[EP_FRAME_LIMITER] = false;
    engineParameters[EP_RESOURCE_PATHS] = "CoreData;Data";
    engineParameters[EP_LOG_NAME] = "ParticleEditor2D.log";
    engineParameters[EP_EXTERNAL_WINDOW] = (void*)(mainWindow_->centralWidget()->winId());
    engineParameters[EP_WINDOW_RESIZABLE] = true;
    engineParameters[EP_FULL_SCREEN] = false;

    if (!engine_->Initialize(engineParameters))
        return -1;

    mainWindow_->CreateWidgets();

    CreateScene();
    CreateConsole();
    CreateDebugHud();

    QTimer timer;
    connect(&timer, SIGNAL(timeout()), this, SLOT(OnTimeout()));
    timer.start(16);

    return QApplication::exec();
}


void ParticleEditor::New()
{
    Open("Urho2D/fire.pex");
    Open("Urho2D/sun2.pex");
    Open("Urho2D/greenspiral.pex");
}


//void ParticleEditor::RemoveSelected()
//{
//    RemoveParticleNode(selectedParticleNodeId_);
//}

bool ParticleEditor::SetVisible(const String& key, bool visible)
{
    auto it = particleNodes_.find(key);
    if (it != particleNodes_.end()) {
        SharedPtr<Node> node = it->second;
//        node->Remove();
//        particleNodes_.erase(it);
        return true;
    }

    return false;
}

bool ParticleEditor::RemoveParticleNode(const String& key)
{
    auto it = particleNodes_.find(key);
    if (it != particleNodes_.end()) {
        SharedPtr<Node> node = it->second;
        scene_->RemoveChild(node);
        node->Remove();
        particleNodes_.erase(it);
        return true;
    }

    return false;
}

bool ParticleEditor::SetParticleNodePosition(const String& key, int x, int y)
{
    auto it = particleNodes_.find(key);
    if (it != particleNodes_.end()) {
        SharedPtr<Node> node = it->second;
        node->SetPosition2D(Vector2(x,y));
        return true;
    }

    return false;
}


bool ParticleEditor::AddParticleNode(const String& fileName)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    ParticleEffect2D* particleEffect = cache->GetResource<ParticleEffect2D>(fileName);
    if (!particleEffect) {
        ErrorExit("Open particle effect failed " + fileName);
        return false;
    }

    SharedPtr<Node> node = SharedPtr<Node>(scene_->CreateChild("ParticleEmitter2D"));
    ParticleEmitter2D* particleEmitter = node->CreateComponent<ParticleEmitter2D>();
    particleEmitter->SetEffect(particleEffect);

    particleNodes_.insert(std::make_pair(fileName, node));

    // cache active
    if (!particleNode_) {
        particleNode_ = node;
        fileName_ = fileName;
    }
    QString key(fileName.CString());
    qInfo()<<"before send="<<key;
    emit NewParticleNodeAdded(key);
    return true;
}

void ParticleEditor::Open(const String& fileName)
{
    bool result = AddParticleNode(fileName);
    if (result) {
        mainWindow_->UpdateWidget();
    }
}

void ParticleEditor::Save(const String& fileName)
{
    ParticleEffect2D* particleEffect = GetEffect();
    if (!particleEffect)
        return;

    File file(context_);
    if (!file.Open(fileName, FILE_WRITE))
    {
        ErrorExit("Open file failed " + fileName);
        return;
    }

    particleEffect->Save(file);

    fileName_ = fileName;
}

Camera* ParticleEditor::GetCamera() const
{
    return cameraNode_->GetComponent<Camera>();
}

ParticleEffect2D* ParticleEditor::GetEffect() const
{
    ParticleEmitter2D* emitter = GetEmitter();
    if (!emitter)
        return 0;

    return emitter->GetEffect();
}


ParticleEmitter2D* ParticleEditor::GetEmitter() const
{
    return particleNode_->GetComponent<ParticleEmitter2D>();
}


ParticleEditor* ParticleEditor::Get()
{
    return qobject_cast<ParticleEditor*>(qApp);
}

void ParticleEditor::OnTimeout()
{
    if (engine_ && !engine_->IsExiting())
        engine_->RunFrame();
}

void ParticleEditor::CreateScene()
{
    scene_ = new Scene(context_);
    scene_->CreateComponent<Octree>();
    scene_->CreateComponent<DebugRenderer>();

    // Create camera.
    cameraNode_ = scene_->CreateChild("Camera");
    Camera* camera = cameraNode_->CreateComponent<Camera>();

    camera->SetOrthographic(true);
    Graphics* graphic = GetSubsystem<Graphics>();
    camera->SetOrthoSize(graphic->GetHeight() * PIXEL_SIZE);

    SharedPtr<Viewport> viewport(new Viewport(context_, scene_, camera));

    Renderer* renderer = GetSubsystem<Renderer>();
    renderer->SetViewport(0, viewport);

    New();
}

void ParticleEditor::CreateConsole()
{
    // Get default style
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    XMLFile* xmlFile = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");

    // Create console
    Console* console = engine_->CreateConsole();
    console->SetDefaultStyle(xmlFile);
}

void ParticleEditor::CreateDebugHud()
{
    // Get default style
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    XMLFile* xmlFile = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");

    // Create debug HUD.
    DebugHud* debugHud = engine_->CreateDebugHud();
    debugHud->SetDefaultStyle(xmlFile);
}

void ParticleEditor::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
    using namespace Update;

    // Take the frame time step, which is stored as a float
    float timeStep = eventData[P_TIMESTEP].GetFloat();
    Input* input = GetSubsystem<Input>();

    // When left button is down, move mouse to particle node
    if (input->GetMouseButtonDown(MOUSEB_LEFT) && particleNode_)
    {
        IntVector2 mousePosition = input->GetMousePosition();

        Graphics* graphics = GetSubsystem<Graphics>();
        Vector3 screenPoint((float)mousePosition.x_ / graphics->GetWidth(), (float)mousePosition.y_ / graphics->GetHeight(), 0.0f);

        Camera* camera = cameraNode_->GetComponent<Camera>();
        Vector3 worldPoint = camera->ScreenToWorldPoint(screenPoint);
        particleNode_->SetPosition(worldPoint);
    }
}

void ParticleEditor::HandleKeyDown(StringHash eventType, VariantMap& eventData)
{
    using namespace KeyDown;
    int key = eventData[P_KEY].GetInt();
    if (key == KEY_F1)
        GetSubsystem<Console>()->Toggle();
    else if (key == KEY_F2)
        GetSubsystem<DebugHud>()->ToggleAll();
}

void ParticleEditor::HandleMouseWheel(StringHash eventType, VariantMap& eventData)
{
    using namespace MouseWheel;

    int wheel = eventData[P_WHEEL].GetInt();
    Camera* camera = cameraNode_->GetComponent<Camera>();
    if (wheel > 0)
        camera->SetZoom(camera->GetZoom() * 1.25f);
    else
        camera->SetZoom(camera->GetZoom() * 0.80f);
}

void ParticleEditor::HandleRenderUpdate(StringHash eventType, VariantMap& eventData)
{
    Camera* camera = cameraNode_->GetComponent<Camera>();
    const float value = 20.0f / camera->GetZoom();
    const Color color(0.0f, 0.5f, 0.0f, 0.5f);
    
    DebugRenderer* debugRenderer = scene_->GetComponent<DebugRenderer>();
    debugRenderer->AddLine(Vector3(-value, 0.0f, 1.0f), Vector3(value, 0.0f, 1.0f), color);
    debugRenderer->AddLine(Vector3(0.0f, -value, 1.0f), Vector3(0.0f, value, 1.0f), color);
    debugRenderer->Render();
}

}
