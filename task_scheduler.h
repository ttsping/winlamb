#pragma once
#include <taskschd.h>
#include <versionhelpers.h>
#include <string>
#include "com.h"
#include "exception.h"

#pragma comment(lib, "taskschd.lib")

namespace wl {

struct task_sch_configuration {
    struct {
        std::wstring author;
        std::wstring description;
    } registration_info;

    struct {
        std::wstring app_path;
        std::wstring arguments;
        std::wstring working_dir;
    } action;

    struct {
        std::wstring group_id;
        std::wstring user_id;
        TASK_RUNLEVEL_TYPE run_level = TASK_RUNLEVEL_HIGHEST;
        TASK_LOGON_TYPE logon_type = TASK_LOGON_INTERACTIVE_TOKEN;
    } principal;

    struct {
        bool allow_demand_start;
        bool allow_hard_terminate;
        bool disallow_start_if_on_batteries;
        bool enabled;
        bool hidden;
        int32_t priority = 0;
        int32_t restart_count = 0;
        bool run_only_if_network_available;
        bool start_when_available;
        bool stop_if_going_on_batteries;
        bool wake_to_run;
        std::wstring execution_time_limit;
        std::wstring delete_expired_task_after;
        std::wstring restart_interval;
        TASK_INSTANCES_POLICY instances_policy = TASK_INSTANCES_IGNORE_NEW;
    } settings;

    TASK_TRIGGER_TYPE2 trigger_type;
};

struct task_sch_run_params {
    std::wstring app_path; // for verified purpose.
    com::variant params;
    long flags;
    long session_id;
    std::wstring user;
};

struct task_sch_information {
    std::wstring name;
    std::wstring description;
    std::wstring app_path;
};

class task_scheduler {
  public:
    task_scheduler(std::wstring_view root = L"\\") : root_(root) {}
    ~task_scheduler() { finalize(); }

    /// <summary>
    /// Initalize task service
    /// </summary>
    /// <param name="create_folder">if folder not exist, set to true will try to create it</param>
    /// <returns>true if successfully</returns>
    bool initalize(bool create_folder = false) noexcept {
        VariantInit(&empty_var_);
        if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED)))
            return false;
        com_init_ = true;
        if (FAILED(CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_PKT_PRIVACY, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, 0, NULL)))
            return false;
        try {
            task_svc_ = com::co_create_instance<ITaskService>(CLSID_TaskScheduler, IID_ITaskService);
            CHECK_HRESULT(task_svc_->Connect(empty_var_, empty_var_, empty_var_, empty_var_));
            HRESULT hr = task_svc_->GetFolder(root_, &task_folder_);
            if (FAILED(hr)) {
                if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND) && create_folder) {
                    com::ptr<ITaskFolder> root_folder;
                    CHECK_HRESULT(task_svc_->GetFolder(com::bstr(L"\\"), &root_folder));
                    CHECK_HRESULT(root_folder->CreateFolder(root_, empty_var_, &task_folder_));
                }
            }
        } catch (std::exception&) {
            return false;
        }
        return true;
    }

    /// <summary>
    /// Finalize task service.
    /// </summary>
    /// <returns></returns>
    void finalize() noexcept {
        if (task_folder_) {
            task_folder_.release();
        }

        if (task_svc_) {
            task_svc_.release();
        }

        if (com_init_) {
            CoUninitialize();
        }
    }

    /// <summary>
    /// Register(Create or Update) a task
    /// </summary>
    /// <param name="task_name">Task name to register</param>
    /// <param name="task_config">Register configurtion</param>
    void register_task(std::wstring_view task_name, const task_sch_configuration& task_config) {
        if (task_name.empty() || task_config.action.app_path.empty()) {
            THROW_INVALID_PARAM_EXCEPTION();
        }
        if (!check_service()) {
            throw std::runtime_error("not initalized.");
        }

        com::ptr<ITaskDefinition> task_def;
        com::ptr<IRegistrationInfo> task_reginfo;
        com::ptr<IPrincipal> task_pricipal;
        com::ptr<ITaskSettings> task_settings;
        com::ptr<IActionCollection> action_collection;
        com::ptr<IAction> task_action;
        com::ptr<IExecAction> exec_action;
        com::ptr<ITriggerCollection> trigger_collection;
        com::ptr<ITrigger> task_trigger;
        com::ptr<IRegisteredTask> registered_task;

        CHECK_HRESULT(task_svc_->NewTask(0, &task_def));
        CHECK_HRESULT(task_def->get_Actions(&action_collection));

        CHECK_HRESULT(action_collection->Create(TASK_ACTION_EXEC, &task_action));
        exec_action = task_action.query_interface<IExecAction>(IID_IExecAction);

        // Action
        auto& action = task_config.action;
        CHECK_HRESULT(exec_action->put_Path(com::bstr(action.app_path)));
        if (!action.arguments.empty()) {
            CHECK_HRESULT(exec_action->put_Arguments(com::bstr(action.arguments)));
        } else {
        }
        if (!action.working_dir.empty()) {
            CHECK_HRESULT(exec_action->put_WorkingDirectory(com::bstr(action.working_dir)));
        }

        // Principal
        auto& principal = task_config.principal;
        CHECK_HRESULT(task_def->get_Principal(&task_pricipal));

        CHECK_HRESULT(task_pricipal->put_RunLevel(principal.run_level));
        CHECK_HRESULT(task_pricipal->put_LogonType(principal.logon_type));
        if (!principal.group_id.empty()) {
            CHECK_HRESULT(task_pricipal->put_GroupId(com::bstr(principal.group_id)));
        }
        if (!principal.user_id.empty()) {
            CHECK_HRESULT(task_pricipal->put_UserId(com::bstr(principal.user_id)));
        }

        // RegistrationInfo
        auto& registration_info = task_config.registration_info;
        if (!registration_info.author.empty() || !registration_info.description.empty()) {
            CHECK_HRESULT(task_def->get_RegistrationInfo(&task_reginfo));
            if (!registration_info.author.empty()) {
                CHECK_HRESULT(task_reginfo->put_Author(com::bstr(registration_info.author)));
            }
            if (!registration_info.description.empty()) {
                CHECK_HRESULT(task_reginfo->put_Description(com::bstr(registration_info.description)));
            }
        }

        // Settings
        auto& settings = task_config.settings;
        CHECK_HRESULT(task_def->get_Settings(&task_settings));
        CHECK_HRESULT(task_settings->put_AllowDemandStart(VAR_BOOL(settings.allow_demand_start)));
        CHECK_HRESULT(task_settings->put_AllowHardTerminate(VAR_BOOL(settings.allow_hard_terminate)));
        CHECK_HRESULT(task_settings->put_DisallowStartIfOnBatteries(VAR_BOOL(settings.disallow_start_if_on_batteries)));
        CHECK_HRESULT(task_settings->put_Enabled(VAR_BOOL(settings.enabled)));
        CHECK_HRESULT(task_settings->put_Hidden(VAR_BOOL(settings.hidden)));
        CHECK_HRESULT(task_settings->put_Priority(settings.priority));
        CHECK_HRESULT(task_settings->put_RestartCount(settings.restart_count));
        CHECK_HRESULT(task_settings->put_RunOnlyIfNetworkAvailable(VAR_BOOL(settings.run_only_if_network_available)));
        CHECK_HRESULT(task_settings->put_StartWhenAvailable(VAR_BOOL(settings.start_when_available)));
        CHECK_HRESULT(task_settings->put_StopIfGoingOnBatteries(VAR_BOOL(settings.stop_if_going_on_batteries)));
        CHECK_HRESULT(task_settings->put_WakeToRun(VAR_BOOL(settings.wake_to_run)));
        CHECK_HRESULT(task_settings->put_MultipleInstances(settings.instances_policy));
        if (IsWindows7OrGreater()) {
            for (int i = TASK_COMPATIBILITY_V2_4; i >= TASK_COMPATIBILITY_V1; --i) {
                if (SUCCEEDED(task_settings->put_Compatibility((TASK_COMPATIBILITY)i)))
                    break;
            }
        }
        if (settings.execution_time_limit.empty()) {
            CHECK_HRESULT(task_settings->put_ExecutionTimeLimit(com::bstr(L"PT0S")));
        } else {
            CHECK_HRESULT(task_settings->put_ExecutionTimeLimit(com::bstr(settings.execution_time_limit)));
        }
        if (!settings.delete_expired_task_after.empty()) {
            CHECK_HRESULT(task_settings->put_DeleteExpiredTaskAfter(com::bstr(settings.delete_expired_task_after)));
        }
        if (!settings.restart_interval.empty()) {
            CHECK_HRESULT(task_settings->put_RestartInterval(com::bstr(settings.restart_interval)));
        }

        // Trigger
        switch (task_config.trigger_type) {
            case TASK_TRIGGER_BOOT:
            case TASK_TRIGGER_LOGON:
                CHECK_HRESULT(task_def->get_Triggers(&trigger_collection));
                CHECK_HRESULT(trigger_collection->Create(task_config.trigger_type, &task_trigger));
                CHECK_HRESULT(task_trigger->put_Enabled(VARIANT_TRUE));
                break;

            default:
                break;
        }

        // Finally! register the task
        CHECK_HRESULT(task_folder_->RegisterTaskDefinition(com::bstr(task_name), task_def, TASK_CREATE_OR_UPDATE, empty_var_, empty_var_,
                                                           TASK_LOGON_INTERACTIVE_TOKEN, empty_var_, &registered_task));
    }

    /// <summary>
    /// Delete a task
    /// </summary>
    /// <param name="task_name">Task name to delete</param>
    /// <returns>true if successfully.</returns>
    bool delete_task(std::wstring_view task_name) noexcept {
        if (task_name.empty()) {
            return false;
        }
        if (!check_service()) {
            return false;
        }
        HRESULT hr = S_OK;
        int num_retries = 0;
        do {
            hr = task_folder_->DeleteTask(com::bstr(task_name), 0);
            if (SUCCEEDED(hr))
                break;
            bool task_registered = is_registered(task_name);
            if (++num_retries > 3)
                break;
            if ((hr == HRESULT_FROM_WIN32(ERROR_TRANSACTION_NOT_ACTIVE) || hr == HRESULT_FROM_WIN32(ERROR_TRANSACTION_ALREADY_ABORTED) && task_registered)) {
                Sleep(50);
                continue;
            }
            if (!task_registered) {
                hr = S_OK;
                break;
            }
        } while (TRUE);
        return SUCCEEDED(hr);
    }

    /// <summary>
    /// Check if a task is registered.
    /// </summary>
    /// <param name="task_name"></param>
    /// <returns></returns>
    FORCEINLINE bool is_registered(std::wstring_view task_name) noexcept {
        if (!check_service()) {
            return false;
        }
        com::ptr<IRegisteredTask> task;
        return get_task(task_name, &task);
    }

    /// <summary>
    /// Get interface of a registered Task
    /// </summary>
    /// <param name="task_name"></param>
    /// <param name="registered_task"></param>
    /// <returns></returns>
    FORCEINLINE bool get_task(std::wstring_view task_name, IRegisteredTask** registered_task) noexcept {
        if (!check_service()) {
            return false;
        }
        return SUCCEEDED(task_folder_->GetTask(com::bstr(task_name), registered_task));
    }

    /// <summary>
    /// Run a registered task
    /// </summary>
    /// <param name="task_name">Task to run</param>
    /// <param name="run_params">Optional, Run parameters.</param>
    void run_task(std::wstring_view task_name, const task_sch_run_params* run_params = nullptr) {
        if (!check_service()) {
            throw std::runtime_error("not initalized.");
        }
        com::ptr<IRegisteredTask> registered_task;
        if (!get_task(task_name, &registered_task)) {
            throw std::runtime_error("get registered task failed");
        }
        com::variant params;
        com::bstr user;
        long flags = TASK_RUN_AS_SELF | TASK_RUN_IGNORE_CONSTRAINTS;
        long session_id = 0;
        if (run_params) {
            if (!run_params->app_path.empty()) {
                task_sch_configuration config;
                get_task_configuration(task_name, config);
                if (!str::eqi(run_params->app_path, config.action.app_path)) {
                    throw std::runtime_error("app path verified failed.");
                }
            }
            if (run_params->flags > 0) {
                flags = run_params->flags;
            }
            if (flags & TASK_RUN_USE_SESSION_ID) {
                session_id = run_params->session_id;
            }
            if (!run_params->user.empty()) {
                user = run_params->user;
            }
            params = run_params->params.clone();
        }

        com::ptr<IRunningTask> running_task;
        CHECK_HRESULT(registered_task->RunEx(params, flags, session_id, user, &running_task));
        int task_run_retry = 3;
        do {
            TASK_STATE state;
            (void)running_task->Refresh();
            HRESULT hr = running_task->get_State(&state);
            if (SUCCEEDED(hr)) {
                if (state == TASK_STATE_RUNNING || state == TASK_STATE_DISABLED) {
                    break;
                }
            }
            --task_run_retry;
        } while (task_run_retry > 0);
    }

    /// <summary>
    /// Get configuration of a registered task.
    /// </summary>
    /// <param name="task_name">Task to get configuration</param>
    /// <param name="config">Output configuration</param>
    void get_task_configuration(std::wstring_view task_name, task_sch_configuration& config) {
        if (!check_service()) {
            throw std::runtime_error("not initalized.");
        }
        com::bstr text;
        VARIANT_BOOL val_bool;
        long col_count = 0;
        com::ptr<IRegisteredTask> registered_task;
        com::ptr<ITaskDefinition> task_def;
        com::ptr<IRegistrationInfo> task_reginfo;
        com::ptr<IActionCollection> action_collection;
        com::ptr<IAction> action;
        com::ptr<IExecAction> exec_action;
        com::ptr<IPrincipal> task_principal;
        com::ptr<ITaskSettings> task_settings;
        com::ptr<ITriggerCollection> trigger_collection;
        com::ptr<ITrigger> trigger;

        if (!get_task(task_name, &registered_task)) {
            throw std::runtime_error("get registered task failed.");
        }
        CHECK_HRESULT(registered_task->get_Definition(&task_def));
        // Registration info
        CHECK_HRESULT(task_def->get_RegistrationInfo(&task_reginfo));
        CHECK_HRESULT(task_reginfo->get_Author(&text));
        text.assign_and_free(config.registration_info.author);
        CHECK_HRESULT(task_reginfo->get_Description(&text));
        text.assign_and_free(config.registration_info.description);
        // Action
        CHECK_HRESULT(task_def->get_Actions(&action_collection));
        CHECK_HRESULT(action_collection->get_Count(&col_count));
        if (col_count > 0) {
            CHECK_HRESULT(action_collection->get_Item(1, &action));
            exec_action = action.query_interface<IExecAction>(IID_IExecAction);
            CHECK_HRESULT(exec_action->get_Path(&text));
            text.assign_and_free(config.action.app_path);
            CHECK_HRESULT(exec_action->get_Arguments(&text));
            text.assign_and_free(config.action.arguments);
            CHECK_HRESULT(exec_action->get_WorkingDirectory(&text));
            text.assign_and_free(config.action.working_dir);
        }
        // Principal
        CHECK_HRESULT(task_def->get_Principal(&task_principal));
        CHECK_HRESULT(task_principal->get_GroupId(&text));
        text.assign_and_free(config.principal.group_id);
        CHECK_HRESULT(task_principal->get_UserId(&text));
        text.assign_and_free(config.principal.user_id);
        CHECK_HRESULT(task_principal->get_RunLevel(&config.principal.run_level));
        CHECK_HRESULT(task_principal->get_LogonType(&config.principal.logon_type));
        // Settings
        CHECK_HRESULT(task_def->get_Settings(&task_settings));
        CHECK_HRESULT(task_settings->get_AllowDemandStart(&val_bool));
        config.settings.allow_demand_start = TO_BOOL(val_bool);
        CHECK_HRESULT(task_settings->get_AllowHardTerminate(&val_bool));
        config.settings.allow_hard_terminate = TO_BOOL(val_bool);
        CHECK_HRESULT(task_settings->get_DisallowStartIfOnBatteries(&val_bool));
        config.settings.disallow_start_if_on_batteries = TO_BOOL(val_bool);
        CHECK_HRESULT(task_settings->get_Enabled(&val_bool));
        config.settings.enabled = TO_BOOL(val_bool);
        CHECK_HRESULT(task_settings->get_Hidden(&val_bool));
        config.settings.hidden = TO_BOOL(val_bool);
        CHECK_HRESULT(task_settings->get_Priority(&config.settings.priority));
        CHECK_HRESULT(task_settings->get_RestartCount(&config.settings.restart_count));
        CHECK_HRESULT(task_settings->get_RunOnlyIfNetworkAvailable(&val_bool));
        config.settings.run_only_if_network_available = TO_BOOL(val_bool);
        CHECK_HRESULT(task_settings->get_StartWhenAvailable(&val_bool));
        config.settings.start_when_available = TO_BOOL(val_bool);
        CHECK_HRESULT(task_settings->get_StopIfGoingOnBatteries(&val_bool));
        config.settings.stop_if_going_on_batteries = TO_BOOL(val_bool);
        CHECK_HRESULT(task_settings->get_WakeToRun(&val_bool));
        config.settings.wake_to_run = TO_BOOL(val_bool);
        CHECK_HRESULT(task_settings->get_ExecutionTimeLimit(&text));
        text.assign_and_free(config.settings.execution_time_limit);
        CHECK_HRESULT(task_settings->get_DeleteExpiredTaskAfter(&text));
        text.assign_and_free(config.settings.delete_expired_task_after);
        CHECK_HRESULT(task_settings->get_RestartInterval(&text));
        text.assign_and_free(config.settings.restart_interval);
        CHECK_HRESULT(task_settings->get_MultipleInstances(&config.settings.instances_policy));
        CHECK_HRESULT(task_def->get_Triggers(&trigger_collection));
        CHECK_HRESULT(trigger_collection->get_Count(&col_count));
        if (col_count > 0) {
            CHECK_HRESULT(trigger_collection->get_Item(1, &trigger));
            CHECK_HRESULT(trigger->get_Type(&config.trigger_type));
        }
    }

  private:
    FORCEINLINE bool check_service() { return task_svc_ && task_folder_; }

  private:
    bool com_init_ = false;
    VARIANT empty_var_;
    com::bstr root_;
    com::ptr<ITaskService> task_svc_;
    com::ptr<ITaskFolder> task_folder_;
};

}  // namespace wl