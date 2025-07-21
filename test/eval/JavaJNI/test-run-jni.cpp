//#define BOOST_TEST_MAIN
//#include <boost/test/unit_test.hpp>

#include <string>
#include <iostream>
#include <jni.h>

using namespace std;


//BOOST_AUTO_TEST_CASE( test_java_jni_method_execution )
int main() {
    JavaVM *jvm;
    JNIEnv *env;

    JavaVMOption* options = new JavaVMOption[1];
    string JavaClassPath = "-Djava.class.path=.";
    options[0].optionString = (char*)JavaClassPath.c_str();

    JavaVMInitArgs vm_args;
    vm_args.version = JNI_VERSION_1_6;
    vm_args.nOptions = 1;
    vm_args.options = options;
    vm_args.ignoreUnrecognized = false;

    jint rc = JNI_CreateJavaVM(&jvm, (void**)&env, &vm_args);
    cout << "JNI CreateJavaVM rc:" << rc << endl;
    delete[] options;

    cout << "JVM load succeeded: Version ";
    jint ver = env->GetVersion();
    cout << ((ver>>16)&0x0f) << "."<<(ver&0x0f) << endl;

    jvm->DestroyJavaVM();
}
