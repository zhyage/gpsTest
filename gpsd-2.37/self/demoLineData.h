#ifndef DEMO_LINE_DATA
#define DEMO_LINE_DATA

typedef struct
{
    double lat;
    double lng; 
}demoData_t;


demoData_t *getLine1DemoData(long i);
long getNumOfLine1DemoData();
demoData_t *getLine44DemoData(long i);
long getNumOfLine44DemoData();

#endif
