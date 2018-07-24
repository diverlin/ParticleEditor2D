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

#include <Urho3D/Core/Object.h>
#include <Urho3D/Container/Ptr.h>

#include <QApplication>

namespace Urho3D
{

class ScaleDownAnimation {
public:

    bool isActive() const { return m_isActive; }
    float current() const { return m_current; }

    void start() {
        m_isActive = true;
        m_current = m_top;
    }

    void update() {
        if (m_current > m_low) {
            m_current -= m_step;
        } else {
            stop();
        }
    }

private:
    float m_current = 1.0f;
    float m_low = 0.0f;
    float m_top = 1.0f;
    float m_step = 0.01f;
    bool m_isActive = false;

    void stop() {
        m_isActive = false;
        m_current = m_low;
    }
};

class Camera;
class Context;
class Engine;
class MainWindow;
class Node;
class ParticleEffect2D;
class ParticleEmitter2D;
class Scene;

/// Particle editor class.
class ParticleEditor : public QApplication, public Object
{
    Q_OBJECT
    URHO3D_OBJECT(ParticleEditor, Object)

public:
    /// Construct.
    ParticleEditor(int argc, char** argv, Context* context);
    /// Destruct.
    virtual ~ParticleEditor();

    /// Run.
    int Run();

    bool SetVisible(const String& key, bool visible);
    bool RemoveParticleNode(const String& key);
    bool SetParticleNodePosition(const String& key, int x, int y);

    bool Open(QString fileName);
    bool Save(const String& fileName);
    bool changeKey(const String& fromKey, const String& toKey);
    bool select(const String& key);

    const String& GetFileName() const { return selectedKey_; }
    /// Return camera.
    Camera* GetCamera() const;
    /// Return effect.
    ParticleEffect2D* GetEffect(const String&) const;
    /// Return emitter.
    ParticleEmitter2D* GetEmitter(const String&) const;

    QList<QString> GetKeys() const;

    /// Return editor pointer.
    static ParticleEditor* Get();

signals:
    void NewParticleNodeAdded(QString);

private slots:
    // Timeout handler.
    void OnTimeout();

private:
    /// Create scene.
    void CreateScene();
    /// Create console.
    void CreateConsole();
    /// Create debug HUD.
    void CreateDebugHud();
    /// Handle update event.
    void HandleUpdate(StringHash eventType, VariantMap& eventData);
    /// Handle key down (toggle debug HUD).
    void HandleKeyDown(StringHash eventType, VariantMap& eventData);
    /// Handle mouse wheel.
    void HandleMouseWheel(StringHash eventType, VariantMap& eventData);
    /// Handle render update.
    void HandleRenderUpdate(StringHash eventType, VariantMap& eventData);

//    void RemoveSelected();
    bool AddParticleNode(const String&);

    /// Editor main window.
    MainWindow* mainWindow_;
    /// Engine.
    SharedPtr<Engine> engine_;
    /// Scene.
    SharedPtr<Scene> scene_;
    /// Camera node.
    SharedPtr<Node> cameraNode_;
    /// Particle nodes <filename, Node>.
    std::map<String, SharedPtr<Node>> particleNodes_;

    ScaleDownAnimation selectedAnimation_;
    String selectedKey_;
    SharedPtr<Node> selectedParticleNode_;
    SharedPtr<Node> selectedEffectNode_;

    void CreateParticles();

    bool isFileAlreadyOpened(const QString& key) const;
    bool renameFile(const QString& fromKey, const QString& toKey) const;
};

} // namespace Urho3D
