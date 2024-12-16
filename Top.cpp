#include "Top.hpp"

Top * Top::myInstance = nullptr;

#ifdef Q_OS_ANDROID
static void logJavaResponse(JNIEnv *env, jobject /*thiz*/, jstring value)
{
    QString javaMsg(env->GetStringUTFChars(value, nullptr));
    if (javaMsg == QString("((((Prev))))")) {
        Top::myInstance->changePlay(false);
    } else if (javaMsg == QString("((((Play))))")) {
        Top::myInstance->play();
    } else if (javaMsg == QString("((((Pause))))")) {
        Top::myInstance->pause();
    } else if (javaMsg == QString("((((Next))))")) {
        Top::myInstance->changePlay(true);
    }

    // qDebug() << "the message from java" << env->GetStringUTFChars(value, nullptr);
}
#endif

Top::Top(QObject * parent) :
    Directory{parent},
    ChangePlay{parent},
    Interval{parent},
    Player{parent}
    #ifdef Q_OS_ANDROID
    , javaObject{"com.denkplay.states.DenkService"}
    #endif
{
    #ifdef Q_OS_ANDROID
    JNINativeMethod methods[] {{"sendToQt", "(Ljava/lang/String;)V", reinterpret_cast<void *>(logJavaResponse)}};

    QJniEnvironment env;
    jclass objectClass = env->GetObjectClass(javaObject.object<jobject>()); // this is simply peeling the class definition from the running object. Can be assigned to "jclass clazz"
    env->RegisterNatives(objectClass,
                         methods,
                         sizeof(methods) / sizeof(methods[0]));
    env->DeleteLocalRef(objectClass);


    auto activity = QJniObject(QNativeInterface::QAndroidApplication::context());
    QAndroidIntent serviceIntent(activity.object(),
                                 "com.denkplay.states.DenkService");

    QJniObject theStartedService = activity.callObjectMethod(
        "startForegroundService",
        "(Landroid/content/Intent;)Landroid/content/ComponentName;",
        serviceIntent.handle().object() );

    // preserve the Activity to javaObject for future use
    javaObject.swap(activity);
    #endif

    myInstance = this;
}

Top::~Top(){
    // qDebug() << "App is getting Killedffffffff";

    // //
    // checkForBackPress();
}

void Top::killAllThreads() {
    killIntervalThread();

}

// will comment
void Top::checkForBackPress() {
    #ifdef Q_OS_ANDROID
    // auto activity = QJniObject(QNativeInterface::QAndroidApplication::context());
    javaObject.callObjectMethod(
        "myBackPressManager",
        "()V" );
    #endif
}

// will comment
void Top::notifyJavaSeviceAboutPlaying(bool isplaying) {
    #ifdef Q_OS_ANDROID
    javaObject.callObjectMethod(
        "setPlayPauseIconInActivity",
        "(Z)V",
        isplaying );
    #endif
}
