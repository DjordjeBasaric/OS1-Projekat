#include "../h/_thread.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_c.hpp"
#include "../test/printing.hpp"

_thread* _thread::running = nullptr;
int _thread::id_th = -2; //NAPRAVLJENO ZA MAXTHREAD

int _thread::create_thread(thread_t* handle, Body body, void* arg, void* stack_space){

    *handle = new _thread(body, arg, stack_space);
    _sem::open_sem(&(*handle)->semJoin, 0);
    if(*handle != nullptr) return 0;
    return -1;
}


void _thread::thread_dispatch(){
    _thread *old = running;
    if(!old->isFinished() ){ Scheduler::put(old);}//ako se menja a nije gotova stavljam u skedzuler

    running = Scheduler::get();

    //trenutni ra cuvam u old->context, a novi ra uzimam iz running->context i stavljam u ra registar
    if(old != running){
        _thread::contextSwitch(&old->context, &running->context);
    }
}

int _thread::thread_exit() {
    _thread::running->setFinished(true);
    _thread::thread_dispatch();
    return 1;
}

void _thread::threadWrapper() {

    Riscv::popSppSpie();
    running->body(running->arg);
    ::thread_exit(); //mora syscall zato sto je presao na usermode

}

void _thread::join_thread(thread_t* handle) {

    if (!(*handle)->isFinished()) {
        sem_wait((*handle)->semJoin);
    }
}
/*
    while(!(*handle)->isFinished())
        _thread::thread_dispatch();
}
*/
