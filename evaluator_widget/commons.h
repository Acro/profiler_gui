#ifndef COMMONS_H
#define COMMONS_H

struct Data {
    unsigned sec;
    unsigned nsec;
    void* func;
    void* caller;
    unsigned state;
};

#endif // COMMONS_H
