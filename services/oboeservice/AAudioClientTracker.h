/*
 * Copyright (C) 2017 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ANDROID_AAUDIO_AAUDIO_CLIENT_TRACKER_H
#define ANDROID_AAUDIO_AAUDIO_CLIENT_TRACKER_H

#include <map>
#include <mutex>
#include <set>

#include <utils/Singleton.h>

#include <aaudio/AAudio.h>
#include "binding/IAAudioClient.h"
#include "AAudioService.h"

namespace aaudio {

class AAudioClientTracker : public android::Singleton<AAudioClientTracker>{
public:
    AAudioClientTracker();
    ~AAudioClientTracker() = default;

    aaudio_result_t registerClient(pid_t pid, const android::sp<android::IAAudioClient>& client);

    void unregisterClient(pid_t pid);

    aaudio_result_t registerClientStream(pid_t pid,
                                         android::sp<AAudioServiceStreamBase> serviceStream);

    aaudio_result_t unregisterClientStream(pid_t pid,
                                           android::sp<AAudioServiceStreamBase> serviceStream);

    android::AAudioService *getAAudioService() const {
        return mAAudioService;
    }

    void setAAudioService(android::AAudioService *aaudioService) {
        mAAudioService = aaudioService;
    }

private:
    class NotificationClient : public IBinder::DeathRecipient {
    public:
        NotificationClient(pid_t pid);
        virtual ~NotificationClient();

        aaudio_result_t registerClientStream(android::sp<AAudioServiceStreamBase> serviceStream);

        aaudio_result_t unregisterClientStream(android::sp<AAudioServiceStreamBase> serviceStream);

        // IBinder::DeathRecipient
        virtual     void    binderDied(const android::wp<IBinder>& who);

    protected:
        std::mutex                                      mLock;
        const pid_t                                     mProcessId;
        std::set<android::sp<AAudioServiceStreamBase>>  mStreams;
    };

    std::mutex                                       mLock;
    std::map<pid_t, android::sp<NotificationClient>> mNotificationClients;
    android::AAudioService                          *mAAudioService = nullptr;
};

} /* namespace aaudio */

#endif //ANDROID_AAUDIO_AAUDIO_CLIENT_TRACKER_H