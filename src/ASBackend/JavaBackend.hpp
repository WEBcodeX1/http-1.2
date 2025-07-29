#include "../Global.hpp"
#include "../ASProcessHandler.hpp"
#include <jni.h>

namespace Backend {

    class Processor {

    public:

        //- initialization
        static void init(ASProcessHandler* ProcessHandlerPtr)
        {
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
            jint rc = JNI_CreateJavaVM(&ProcessHandlerPtr->jvm, (void**)&ProcessHandlerPtr->jnienv, &vm_args);
            cout << "JNI CreateJavaVM rc:" << rc << endl;
            delete[] options;
        }

        //- process request
        static void process(ASProcessHandler* ProcessHandlerPtr, int Index)
        {
            //- try loading compiled "WebApp" class
            jclass JavaClass = ProcessHandlerPtr->jnienv->FindClass("WebApp");
            if(JavaClass == nullptr) {
                cerr << "ERROR: WebApp class not found!";
            }
            else {
                //- get WebApp.invoke() reference
                jmethodID JavaMethodID = ProcessHandlerPtr->jnienv->GetStaticMethodID(
                    JavaClass,
                    "invoke",
                    "(Ljava/lang/String;)Ljava/lang/String;"
                );

                if(JavaMethodID == nullptr) {
                    cerr << "ERROR: String invoke(String) method not found!" << endl;
                }
                else {
                    jstring InputJSON = ProcessHandlerPtr->jnienv->NewStringUTF(ProcessHandlerPtr->ReqPayloadString);
                    jobject Result = (jstring)ProcessHandlerPtr->jnienv->CallStaticObjectMethod(JavaClass, JavaMethodID, InputJSON);
                    ProcessHandlerPtr->jnienv->ReleaseStringUTFChars(InputJSON, NULL);
                    if(Result != nullptr) {
                        const char* ResultString = ProcessHandlerPtr->jnienv->GetStringUTFChars((jstring)Result, NULL);
                        cout << "Returned payload string:" << ResultString << endl;

                        //- set result payload
                        char* Payload = new(ProcessHandlerPtr->getResultAddress(Index)) char[ProcessHandlerPtr->strlen(ResultString)];
                        memcpy(Payload, &ResultString[0], ProcessHandlerPtr->strlen(ResultString));

                        //- set result payload length
                        new(ProcessHandlerPtr->getMetaAddress(Index, 7)) HTTPPayloadLength_t(ProcessHandlerPtr->strlen(ResultString));
                    }
                }
            }

        }

    };
}
