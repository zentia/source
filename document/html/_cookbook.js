var _cookbook =
[
    [ "Project Motivation", "_project_motivation.html", [
      [ "The Era of Multicore", "_project_motivation.html#TheEraOfMulticore", null ],
      [ "Heterogeneous Computing", "_project_motivation.html#C0HeterogeneousComputing", null ],
      [ "Loop-level Parallelism", "_project_motivation.html#LoopLevelParallelism", null ],
      [ "Task-based Parallelism", "_project_motivation.html#TaskBasedParallelism", null ],
      [ "The Project Mantra", "_project_motivation.html#TheProjectMantra", null ]
    ] ],
    [ "Static Tasking", "_static_tasking.html", [
      [ "Create a Task Dependency Graph", "_static_tasking.html#CreateATaskDependencyGraph", null ],
      [ "Visualize a Task Dependency Graph", "_static_tasking.html#VisualizeATaskDependencyGraph", null ],
      [ "Modify Task Attributes", "_static_tasking.html#ModifyTaskAttributes", null ],
      [ "Traverse Adjacent Tasks", "_static_tasking.html#TraverseAdjacentTasks", null ],
      [ "Attach User Data to a Task", "_static_tasking.html#AttachUserDataToATask", null ],
      [ "Understand the Lifetime of a Task", "_static_tasking.html#UnderstandTheLifetimeOfATask", null ],
      [ "Move a Taskflow", "_static_tasking.html#MoveATaskflow", null ]
    ] ],
    [ "Executor", "_execute_taskflow.html", [
      [ "Create an Executor", "_execute_taskflow.html#CreateAnExecutor", null ],
      [ "Execute a Taskflow", "_execute_taskflow.html#ExecuteATaskflow", null ],
      [ "Execute a Taskflow with Transferred Ownership", "_execute_taskflow.html#ExecuteATaskflowWithTransferredOwnership", null ],
      [ "Execute a Taskflow from an Internal Worker", "_execute_taskflow.html#ExecuteATaskflowFromAnInternalWorker", null ],
      [ "Touch an Executor from Multiple Threads", "_execute_taskflow.html#ThreadSafety", null ],
      [ "Query the Worker ID", "_execute_taskflow.html#QueryTheWorkerID", null ],
      [ "Observe Thread Activities", "_execute_taskflow.html#ObserveThreadActivities", null ],
      [ "Modify Worker Property", "_execute_taskflow.html#ModifyWorkerProperty", null ]
    ] ],
    [ "Subflow Tasking", "_subflow_tasking.html", [
      [ "Create a Subflow", "_subflow_tasking.html#CreateASubflow", null ],
      [ "Join a Subflow", "_subflow_tasking.html#JoinASubflow", null ],
      [ "Detach a Subflow", "_subflow_tasking.html#DetachASubflow", null ],
      [ "Create a Nested Subflow", "_subflow_tasking.html#CreateANestedSubflow", null ]
    ] ],
    [ "Conditional Tasking", "_conditional_tasking.html", [
      [ "Create a Condition Task", "_conditional_tasking.html#CreateAConditionTask", null ],
      [ "Understand our Task-level Scheduling", "_conditional_tasking.html#TaskSchedulingPolicy", [
        [ "Example", "_conditional_tasking.html#TaskLevelSchedulingExample", null ]
      ] ],
      [ "Avoid Common Pitfalls", "_conditional_tasking.html#AvoidCommonPitfalls", null ],
      [ "Implement Control-flow Graphs", "_conditional_tasking.html#ImplementControlFlowGraphs", [
        [ "Implement If-Else Control Flow", "_conditional_tasking.html#ImplementIfElseControlFlow", null ],
        [ "Implement Switch Control Flow", "_conditional_tasking.html#ImplementSwitchControlFlow", null ],
        [ "Implement Do-While-Loop Control Flow", "_conditional_tasking.html#ImplementDoWhileLoopControlFlow", null ],
        [ "Implement While-Loop Control Flow", "_conditional_tasking.html#ImplementWhileLoopControlFlow", null ]
      ] ],
      [ "Create a Multi-condition Task", "_conditional_tasking.html#CreateAMultiConditionTask", null ]
    ] ],
    [ "Composable Tasking", "_composable_tasking.html", [
      [ "Compose a Taskflow", "_composable_tasking.html#ComposeATaskflow", null ],
      [ "Create a Module Task from a Taskflow", "_composable_tasking.html#CreateAModuleTaskFromATaskflow", null ],
      [ "Create a Custom Composable Graph", "_composable_tasking.html#CreateACustomComposableGraph", null ]
    ] ],
    [ "Asynchronous Tasking", "_async_tasking.html", [
      [ "Launch Asynchronous Tasks from an Executor", "_async_tasking.html#LaunchAsynchronousTasksFromAnExecutor", null ],
      [ "Launch Asynchronous Tasks from a Runtime", "_async_tasking.html#LaunchAsynchronousTasksFromARuntime", null ]
    ] ],
    [ "Asynchronous Tasking with Dependencies", "_dependent_async_tasking.html", [
      [ "Create a Dynamic Task Graph", "_dependent_async_tasking.html#CreateADynamicTaskGraph", null ],
      [ "Specify a Range of Dependent Async Tasks", "_dependent_async_tasking.html#SpecifyARagneOfDependentAsyncTasks", null ],
      [ "Understand the Lifetime of a Dependent Async Task", "_dependent_async_tasking.html#UnderstandTheLifeTimeOfADependentAsyncTask", null ],
      [ "Create a Dynamic Task Graph by Multiple Threads", "_dependent_async_tasking.html#CreateADynamicTaskGraphByMultipleThreads", null ],
      [ "Query the Completion Status of Dependent Async Tasks", "_dependent_async_tasking.html#QueryTheComppletionStatusOfDependentAsyncTasks", null ]
    ] ],
    [ "Interact with the Runtime", "_runtime_tasking.html", [
      [ "Create a Runtime Object", "_runtime_tasking.html#CreateARuntimeTask", null ],
      [ "Acquire the Running Executor", "_runtime_tasking.html#AcquireTheRunningExecutor", null ],
      [ "Run a Task Graph Synchronously", "_runtime_tasking.html#RuntimeTaskingRunATaskGraphSynchronously", null ],
      [ "Learn More About Runtime", "_runtime_tasking.html#LearnMoreAboutRuntime", null ]
    ] ],
    [ "Exception Handling", "_exception_handling.html", [
      [ "Catch an Exception from a Running Taskflow", "_exception_handling.html#CatchAnExceptionFromARunningTaskflow", null ],
      [ "Catch an Exception from a Subflow", "_exception_handling.html#CatchAnExceptionFromASubflow", null ],
      [ "Catch an Exception from an Async Task", "_exception_handling.html#CatchAnExceptionFromAnAsyncTask", null ],
      [ "Catch an Exception from a Corun Loop", "_exception_handling.html#CatchAnExceptionFromACorunLoop", null ]
    ] ],
    [ "Limit the Maximum Concurrency", "_limit_the_maximum_concurrency.html", [
      [ "Define a Semaphore", "_limit_the_maximum_concurrency.html#DefineASemaphore", null ],
      [ "Use Semaphores Across Different Tasks", "_limit_the_maximum_concurrency.html#UseSemaphoresAcrossDifferentTasks", null ],
      [ "Define a Conflict Graph", "_limit_the_maximum_concurrency.html#DefineAConflictGraph", null ],
      [ "Reset a Semaphore", "_limit_the_maximum_concurrency.html#ResetASemaphore", null ],
      [ "Understand the Limitation of Semaphores", "_limit_the_maximum_concurrency.html#UnderstandTheLimitationOfSemaphores", null ]
    ] ],
    [ "Request Cancellation", "_request_cancellation.html", [
      [ "Cancel Execution of Taskflows", "_request_cancellation.html#CancelARunningTaskflow", null ],
      [ "Understand the Limitations of Cancellation", "_request_cancellation.html#UnderstandTheLimitationsOfCancellation", null ]
    ] ],
    [ "GPU Tasking (cudaFlow)", "_g_p_u_taskingcuda_flow.html", [
      [ "Include the Header", "_g_p_u_taskingcuda_flow.html#GPUTaskingcudaFlowIncludeTheHeader", null ],
      [ "What is a CUDA Graph?", "_g_p_u_taskingcuda_flow.html#WhatIsACudaGraph", null ],
      [ "Create a cudaFlow", "_g_p_u_taskingcuda_flow.html#Create_a_cudaFlow", null ],
      [ "Compile a cudaFlow Program", "_g_p_u_taskingcuda_flow.html#Compile_a_cudaFlow_program", null ],
      [ "Run a cudaFlow on Specific GPU", "_g_p_u_taskingcuda_flow.html#run_a_cudaflow_on_a_specific_gpu", null ],
      [ "Create Memory Operation Tasks", "_g_p_u_taskingcuda_flow.html#GPUMemoryOperations", null ],
      [ "Offload a cudaFlow", "_g_p_u_taskingcuda_flow.html#OffloadAcudaFlow", null ],
      [ "Update a cudaFlow", "_g_p_u_taskingcuda_flow.html#UpdateAcudaFlow", null ],
      [ "Integrate a cudaFlow into Taskflow", "_g_p_u_taskingcuda_flow.html#IntegrateCudaFlowIntoTaskflow", null ]
    ] ],
    [ "GPU Tasking (cudaFlowCapturer)", "_g_p_u_taskingcuda_flow_capturer.html", [
      [ "Include the Header", "_g_p_u_taskingcuda_flow_capturer.html#GPUTaskingcudaFlowCapturerIncludeTheHeader", null ],
      [ "Capture a cudaFlow", "_g_p_u_taskingcuda_flow_capturer.html#Capture_a_cudaFlow", null ],
      [ "Common Capture Methods", "_g_p_u_taskingcuda_flow_capturer.html#CommonCaptureMethods", null ],
      [ "Create a Capturer on a Specific GPU", "_g_p_u_taskingcuda_flow_capturer.html#CreateACapturerOnASpecificGPU", null ],
      [ "Create a Capturer from a cudaFlow", "_g_p_u_taskingcuda_flow_capturer.html#CreateACapturerWithinAcudaFlow", null ],
      [ "Offload a cudaFlow Capturer", "_g_p_u_taskingcuda_flow_capturer.html#OffloadAcudaFlowCapturer", null ],
      [ "Update a cudaFlow Capturer", "_g_p_u_taskingcuda_flow_capturer.html#UpdateAcudaFlowCapturer", null ],
      [ "Integrate a cudaFlow Capturer into Taskflow", "_g_p_u_taskingcuda_flow_capturer.html#IntegrateCudaFlowCapturerIntoTaskflow", null ]
    ] ],
    [ "Profile Taskflow Programs", "_profiler.html", [
      [ "Enable Taskflow Profiler", "_profiler.html#ProfilerEnableTFProf", null ],
      [ "Enable Taskflow Profiler on a HTTP Server", "_profiler.html#ProfilerEnableTFProfServer", null ],
      [ "Display Profile Summary", "_profiler.html#ProfilerDisplayProfileSummary", null ]
    ] ]
];