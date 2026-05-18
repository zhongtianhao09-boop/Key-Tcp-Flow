#pragma once
#include <queue>
#include <pthread.h>

template <typename T>
class TaskQuene {
public:
    TaskQuene() { pthread_mutex_init(&m_mutex, NULL); }
    ~TaskQuene() { pthread_mutex_destroy(&m_mutex); }

    void push(const T &item) {
        pthread_mutex_lock(&m_mutex);
        m_queue.push(item);
        pthread_mutex_unlock(&m_mutex);
    }

    bool pop(T &item) {
        pthread_mutex_lock(&m_mutex);
        if (m_queue.empty()) {
            pthread_mutex_unlock(&m_mutex);
            return false;
        }
        item = m_queue.front();
        m_queue.pop();
        pthread_mutex_unlock(&m_mutex);
        return true;
    }

    bool empty() {
        pthread_mutex_lock(&m_mutex);
        bool e = m_queue.empty();
        pthread_mutex_unlock(&m_mutex);
        return e;
    }

    size_t size() {
        pthread_mutex_lock(&m_mutex);
        size_t s = m_queue.size();
        pthread_mutex_unlock(&m_mutex);
        return s;
    }

private:
    std::queue<T> m_queue;
    pthread_mutex_t m_mutex;
};
