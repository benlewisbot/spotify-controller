/**
 * @file EventBus.hpp
 * @brief Simple Event Bus for Pub/Sub Pattern
 *
 * A lightweight event bus for decoupling components.
 * Components can publish events and subscribe to events of interest.
 */

#ifndef EVENT_BUS_HPP
#define EVENT_BUS_HPP

#include "State.hpp"
#include <vector>
#include <functional>
#include <map>
#include <algorithm>

/**
 * @brief Event Bus Class
 *
 * Implements a publish-subscribe pattern for event handling.
 * Multiple subscribers can register for the same event type.
 */
class EventBus {
public:
    EventBus() = default;
    ~EventBus() = default;

    // Delete copy constructor and assignment operator
    EventBus(const EventBus&) = delete;
    EventBus& operator=(const EventBus&) = delete;

    /**
     * @brief Subscribe to an event type
     * @param type The event type to subscribe to
     * @param callback The function to call when the event is published
     * @return A unique subscription ID that can be used to unsubscribe
     */
    int subscribe(EventType type, EventCallback callback) {
        int id = nextSubscriptionId++;
        subscribers[type].push_back({id, callback});
        return id;
    }

    /**
     * @brief Unsubscribe from an event type
     * @param subscriptionId The ID returned from subscribe()
     * @return true if subscription was found and removed
     */
    bool unsubscribe(int subscriptionId) {
        for (auto& pair : subscribers) {
            auto& list = pair.second;
            auto it = std::remove_if(list.begin(), list.end(),
                [subscriptionId](const Subscriber& s) {
                    return s.id == subscriptionId;
                });
            if (it != list.end()) {
                list.erase(it, list.end());
                return true;
            }
        }
        return false;
    }

    /**
     * @brief Publish an event to all subscribers
     * @param event The event to publish
     */
    void publish(const Event& event) {
        auto it = subscribers.find(event.type);
        if (it != subscribers.end()) {
            for (const auto& subscriber : it->second) {
                try {
                    subscriber.callback(event);
                } catch (const std::exception& e) {
                    Serial.printf("⚠️  Event callback error: %s\n", e.what());
                } catch (...) {
                    Serial.printf("⚠️  Unknown event callback error\n");
                }
            }
        }

        // Also publish to ALL subscribers (for wildcard handling)
        publishToAll(event);
    }

    /**
     * @brief Subscribe to all events
     * @param callback The function to call for all events
     * @return A unique subscription ID
     */
    int subscribeAll(EventCallback callback) {
        int id = nextSubscriptionId++;
        allSubscribers.push_back({id, callback});
        return id;
    }

    /**
     * @brief Clear all subscribers for a specific event type
     * @param type The event type to clear
     */
    void clear(EventType type) {
        subscribers.erase(type);
    }

    /**
     * @brief Clear all subscribers
     */
    void clearAll() {
        subscribers.clear();
        allSubscribers.clear();
    }

    /**
     * @brief Get the number of subscribers for an event type
     * @param type The event type
     * @return Number of subscribers
     */
    size_t getSubscriberCount(EventType type) const {
        auto it = subscribers.find(type);
        if (it != subscribers.end()) {
            return it->second.size();
        }
        return 0;
    }

private:
    struct Subscriber {
        int id;
        EventCallback callback;
    };

    void publishToAll(const Event& event) {
        for (const auto& subscriber : allSubscribers) {
            try {
                subscriber.callback(event);
            } catch (const std::exception& e) {
                Serial.printf("⚠️  Event callback error (all): %s\n", e.what());
            } catch (...) {
                Serial.printf("⚠️  Unknown event callback error (all)\n");
            }
        }
    }

    std::map<EventType, std::vector<Subscriber>> subscribers;
    std::vector<Subscriber> allSubscribers;
    int nextSubscriptionId = 1;
};

#endif // EVENT_BUS_HPP
