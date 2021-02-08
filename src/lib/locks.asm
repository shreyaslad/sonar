global spinlock_lock
global spinlock_release

spinlock_lock:
    lock bts QWORD [rdi], 0
    jc spin
    ret

spin:
    pause
    test QWORD [rdi], 1
    jnz spin
    jmp spinlock_lock

spinlock_release:
    lock btr QWORD [rdi], 0
    ret