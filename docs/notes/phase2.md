# Phase 2: Multitasking and Context Switching

### Architecture Overview

1. kernel.c
    * main() creates tasks and starts scheduler
    * task_#() &rarr; each task has a task function
    * calls timer_init() to start interrupts

2. tasks.c/tasks.h
    * task_t structure &rarr; task control block
    * tasks[] array &rarr; holds all tasks
    * current_task &rarr; index of current task (in tasks array)
    * task_create() &rarr; create new task

3. timer.c/timer.h
    * timer_init() &rarr; initialize timer for 1ms intervals
    * timer_interrupt_handler() &rarr; called every 1ms

4. context.s
    * context_switch() &rarr; save and restore registers
    * task_exit() &rarr; cleanup when task finishes

```
*** Note: this is a round robin task scheduler ***

Timer fires every 1ms
    ↓
interrupt fires (hardware)
    ↓
timer_interrupt_handler() called
    ↓
get current task, get next task
    ↓
call context_switch() in assembly
    ↓
assembly saves current task's registers
    ↓
assembly loads next task's registers
    ↓
assembly jumps to next task's program counter
    ↓
next task resumes execution
```

### Task Control Block (TCB) Design

```
struct task:
    registers[16] // CPU register 0-15
    sp // stack pointer for task
    tack[SIZE] // actual stack memory for task
    state // READY, RUNNING, BLOCKED, etc
    task_id // task identifier
```
* `registers` array stores all registers so they can be saved/restored
* stack array is the actual chunk of memory the stack of this task uses which the sp (stack pointer) points to the top of
* when initializing a task:
    * registers[15] is the address of the task's function (program counter, task jumps to this address for the current task function)
    * registers[13] is the address of the task's stack (stack pointer)
    * registers[14] is the address of task_exit() (return address)

### Timer Architecture
* BCM2835 ARM Timer (separate from system timer)
* counts down from reload value, on 0 fire an interrupt, repeat
    1. write reload value to `ARM_TIMER_LOAD`
    2. enable timer and interrupts in `ARM_TIMER_CTRL`
    3. each 1ms (for reload=1000 at 1MHz) fire timer
    4. interrupt handler clears interrupt flag
        * clear the interrupt flag (write to `ARM_TIMER_IRQ`)
        * save current task's state
        * pick next task
        * load next task's state
        * jump to next task
    5. timer reloads and counts again

### Context Switching in Assembly
