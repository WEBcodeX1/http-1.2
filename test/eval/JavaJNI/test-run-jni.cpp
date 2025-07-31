#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include <string>
#include <iostream>
#include <jni.h>

using namespace std;


BOOST_AUTO_TEST_CASE( test_java_jni_method_execution )
{
    //- jvm / jnienv ref
    JavaVM *jvm;
    JNIEnv *env;

    //- test vars
    bool JavaVMExecuted = false;

    //- set java class path (jvm options)
    JavaVMOption* options = new JavaVMOption[1];
    string JavaClassPath = "-Djava.class.path=./json-java.jar:.";
    options[0].optionString = (char*)JavaClassPath.c_str();

    //- set jvm init arguments
    JavaVMInitArgs vm_args;
    vm_args.version = JNI_VERSION_1_6;
    vm_args.nOptions = 1;
    vm_args.options = options;
    vm_args.ignoreUnrecognized = false;

    //- init jvm / jni interface
    jint rc = JNI_CreateJavaVM(&jvm, (void**)&env, &vm_args);
    cout << "JNI CreateJavaVM rc:" << rc << endl;
    delete[] options;

    //- output jvm version
    cout << "JVM load succeeded: Version ";
    jint ver = env->GetVersion();
    cout << ((ver>>16)&0x0f) << "."<<(ver&0x0f) << endl;

    //- try loading compiled "WebApp" class
    jclass JavaClass = env->FindClass("WebApp");
    if(JavaClass == nullptr) {
        cerr << "ERROR: WebApp class not found!";
    }
    else {
        //- get WebApp.invoke() reference
        jmethodID JavaMethodID = env->GetStaticMethodID(JavaClass, "invoke", "(Ljava/lang/String;)Ljava/lang/String;");
        if(JavaMethodID == nullptr) {
            cerr << "ERROR: String invoke(String) method not found!" << endl;
        }
        else {
            jstring InputJSON = env->NewStringUTF("{\"payload\": \"test-string\" }");
            jobject Result = (jstring)env->CallStaticObjectMethod(JavaClass, JavaMethodID, InputJSON);
            env->ReleaseStringUTFChars(InputJSON, NULL);
            cout << "Result:" << Result << endl;
            if(Result != nullptr) {
                const char* ResultString = env->GetStringUTFChars((jstring)Result, NULL);
                cout << "Returned payload string:" << ResultString << endl;
                BOOST_TEST(ResultString == "test-string");
                JavaVMExecuted = true;
            }
        }
    }

    //- finally destroy jvm
    jvm->DestroyJavaVM();

    //- check for failed JVM exec
    BOOST_TEST(JavaVMExecuted == true);
}
