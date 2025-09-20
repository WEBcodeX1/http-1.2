#include "../Global.hpp"
#include "../ASProcessHandler.hpp"

#include <memory>
#include <jni.h>


namespace Backend {

    class Processor {

    public:

        //- initialization
        static void init(ASProcessHandler* ProcessHandlerPtr)
        {
            //- set java class path (jvm options)
            auto options = std::make_unique<JavaVMOption[]>(1);
            string JavaClassPath = "-Djava.class.path=./json-java.jar:.";
            options[0].optionString = (char*)JavaClassPath.c_str();

            //- set jvm init arguments
            JavaVMInitArgs vm_args;
            vm_args.version = JNI_VERSION_1_6;
            vm_args.nOptions = 1;
            vm_args.options = options.get();
            vm_args.ignoreUnrecognized = false;

            //- init jvm / jni interface
            jint rc = JNI_CreateJavaVM(&ProcessHandlerPtr->jvm, (void**)&ProcessHandlerPtr->jnienv, &vm_args);
            DBG(160, "JNI CreateJavaVM rc:" << rc);
            // options automatically deallocated when going out of scope
        }

        //- process request
        static void process(ASProcessHandler* ProcessHandlerPtr, int Index)
        {
            DBG(100, "JavaAS Index:" << Index << " call 'invoke(" << ProcessHandlerPtr->ReqPayloadString << ")'.");
            const char* ResultCharArray = "";

            //- try loading compiled "WebApp" class
            jclass JavaClass = ProcessHandlerPtr->jnienv->FindClass("WebApp");
            if(JavaClass == nullptr) {
                ERR("JNI WebApp class not found.");
            }
            else {
                //- get WebApp.invoke() reference
                jmethodID JavaMethodID = ProcessHandlerPtr->jnienv->GetStaticMethodID(
                    JavaClass,
                    "invoke",
                    "(Ljava/lang/String;)Ljava/lang/String;"
                );

                if(JavaMethodID == nullptr) {
                    ERR("JNI invoke() method not found.");
                }
                else {
                    jstring InputJSON = ProcessHandlerPtr->jnienv->NewStringUTF(ProcessHandlerPtr->ReqPayloadString.c_str());
                    jobject Result = (jstring)ProcessHandlerPtr->jnienv->CallStaticObjectMethod(JavaClass, JavaMethodID, InputJSON);
                    ProcessHandlerPtr->jnienv->ReleaseStringUTFChars(InputJSON, NULL);
                    if(Result == nullptr) {
                        ERR("JNI invoke() processing error.");
                        ProcessHandlerPtr->jnienv->ExceptionDescribe();
                    }
                    else {
                        ResultCharArray = ProcessHandlerPtr->jnienv->GetStringUTFChars((jstring)Result, NULL);
                        DBG(160, "Returned payload string:" << ResultCharArray);

                        //- copy result into correct memory region
                        char* Payload = new(ProcessHandlerPtr->getResultAddress(Index)) char[strlen(ResultCharArray)];
                        memcpy(Payload, &ResultCharArray[0], strlen(ResultCharArray));

                        //- set result payload length
                        new(ProcessHandlerPtr->getMetaAddress(Index, 7)) HTTPPayloadLength_t(strlen(ResultCharArray));
                    }
                }
            }

        }

    };
}
