#pragma once
#include <memory>

#include "editor/input/editor_input_module.h"
#include "editor/scene/editor_scene_manager.h"
#include "module/asset/asset_module.h"
#include "module/config/config_module.h"
#include "module/render/render_module.h"
#include "module/time/time_module.h"
#include "module/window/window_module.h"
#include "module/world/world_module.h"
#include "module/ui/ui_module.h"
#include "runtime/runtime/stream.h"

namespace source_runtime
{
    class application
    {
        double m_LaunchTime;

    public:
        explicit application(const std::string&& exe_path);
        virtual ~application();

        void initialize(const std::string& config_file_path);

        virtual void RequestRecreateGfxDevice();
        virtual void RequestLoadRenderDoc();
        virtual void SetSceneRepaintDirty() {}
        virtual void ForceRepaintNextFrame() {}
        std::shared_ptr<source_module::window::window_module>& get_window_system() { return m_window_module; }
        void update() const;
        void clear();
        void begin_frame_render_thread();

        bool m_RecreateGfxDevice;
        bool m_LoadRenderDoc;
        std::shared_ptr<source_module::window::window_module> m_window_module;
        std::shared_ptr<source_runtime::config_module> m_config_module;
        std::shared_ptr<source_module::render::render_module> m_render_module;
        std::shared_ptr<source_runtime::world_module> m_world_module;
        std::shared_ptr<source_editor::editor_scene_manager> m_scene_manager{ nullptr };
        std::shared_ptr<source_editor::editor_input_module> m_input_module{ nullptr };
        std::shared_ptr<source_runtime::ui::ui_module> m_ui_module;
        std::shared_ptr<source_module::time::time_module> m_time_module{ nullptr };
        std::shared_ptr<source_module::asset::asset_module> m_asset_module{ nullptr };
        std::shared_ptr<source_module::rhi::rhi_module> m_rhi_module{ nullptr };
        static application* instance();
    private:
        static application* m_application_;
        luisa::compute::Device m_device_;
        luisa::compute::Stream m_stream_;
    };
}
