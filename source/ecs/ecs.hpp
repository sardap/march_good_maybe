#pragma once

#include <bitset>
#include <concepts>
#include <limits>
#include <optional>
#include <tuple>
#include <unordered_map>

namespace mgm::ecs {

// Thank god for this
// https://stackoverflow.com/questions/16928669/how-to-get-n-th-type-from-a-tuple
template <int N, typename... Ts>
struct get_tuple_type;

template <int N, typename T, typename... Ts>
struct get_tuple_type<N, std::tuple<T, Ts...>> {
    using Type = typename get_tuple_type<N - 1, std::tuple<Ts...>>::Type;
};

template <typename T, typename... Ts>
struct get_tuple_type<0, std::tuple<T, Ts...>> {
    using Type = T;
};

using EntityID = int;

template <typename T>
concept IsComponent = true;

template <class T>
class Mallocator {
   private:
    void report(T* p, std::size_t n, bool alloc = true) const {
        // std::cout << (alloc ? "Alloc: " : "Dealloc: ") << sizeof(T) * n
        //           << " bytes at " << std::hex << std::showbase
        //           << reinterpret_cast<void*>(p) << std::dec << '\n';
    }

    T m_values[100];
    size_t m_top;

   public:
    typedef T value_type;

    Mallocator() = default;

    template <class U>
    constexpr Mallocator(const Mallocator<U>&) noexcept {}

    [[nodiscard]] T* allocate(std::size_t n) {
        // if (n > std::numeric_limits<std::size_t>::max() / sizeof(T))
        //     return nullptr;

        // if (auto p = static_cast<T*>(std::malloc(n * sizeof(T)))) {
        //     report(p, n);
        //     return p;
        // }
        T* result = m_values + m_top;
        m_top += n;
        return result;
        // return m_values;
    }

    void deallocate(T* p, std::size_t n) noexcept {
        report(p, n, 0);
        std::free(p);
    }
};

template <class T, class U>
bool operator==(const Mallocator<T>&, const Mallocator<U>&) {
    return true;
}

template <class T, class U>
bool operator!=(const Mallocator<T>&, const Mallocator<U>&) {
    return false;
}

template <IsComponent T, int POOL_SIZE, int ENTITY_COUNT>
class CompetentPool {
   private:
    using IndexMap =
        std::unordered_map<EntityID, int, std::hash<EntityID>,
                           std::equal_to<EntityID>,
                           Mallocator<std::pair<const EntityID, int>>>;

    std::optional<T> m_pool[POOL_SIZE];
    s8 m_mapping[ENTITY_COUNT];

    int get_entry_idx(EntityID entity_id) { return m_mapping[entity_id]; }

   public:
    using CONTAINS_TYPE = T;

    CompetentPool() {
        for (int i = 0; i < POOL_SIZE; i++) {
            m_pool[i] = std::nullopt;
        }

        for (int i = 0; i < ENTITY_COUNT; i++) {
            m_mapping[i] = -1;
        }
    }

    void emplace(EntityID entity_id, T new_com) {
        for (int i = 0; i < POOL_SIZE; i++) {
            if (!m_pool[i].has_value()) {
                m_mapping[entity_id] = i;
                m_pool[i] = new_com;
                break;
            }
        }
    }

    void remove(EntityID entity_id) {
        int idx = get_entry_idx(entity_id);
        if (idx != -1) {
            m_pool[idx] = std::nullopt;
        }
    }

    void clear() {
        for (int i = 0; i < POOL_SIZE; i++) {
            m_pool[i] = std::nullopt;
        }
    }

    std::reference_wrapper<CONTAINS_TYPE> get(EntityID entity_id) {
        int idx = get_entry_idx(entity_id);
        return *m_pool[idx];
    }

    bool has(EntityID entity_id) {
        int idx = get_entry_idx(entity_id);
        return idx != -1;
    }
};

template <int ENTITY_LIMIT, typename... POOLS>
class Registry {
   private:
    using Pools = std::tuple<POOLS...>;
    Pools m_pools;
    std::bitset<ENTITY_LIMIT> m_entities;

    template <IsComponent COM, size_t I = 0>
    static constexpr u32 get_pool_idx() {
        using PoolType = get_tuple_type<I, Pools>::Type::CONTAINS_TYPE;

        if constexpr (std::same_as<PoolType, COM>) {
            return I;
        }

        if constexpr (I + 1 != sizeof...(POOLS)) {
            return get_pool_idx<COM, I + 1>();
        }

        return I;
    }

    template <IsComponent COMPONENT>
    std::reference_wrapper<COMPONENT> get_component_for_entity(
        EntityID entity_id) {
        auto& pool = std::get<get_pool_idx<COMPONENT>()>(m_pools);
        return pool.get(entity_id);
    }

   public:
    Registry() { m_entities = 0; }

    EntityID create() {
        for (int i = 0; i < ENTITY_LIMIT; i++) {
            if (!m_entities[i]) {
                m_entities[i] = true;
                return static_cast<EntityID>(i);
            }
        }

        return 0;
    }

    template <IsComponent COM>
    void emplace(EntityID entity_id, COM com) {
        auto* pool = &std::get<get_pool_idx<COM>()>(m_pools);
        pool->emplace(entity_id, com);
    }

    template <size_t I = 0>
    void destroy(EntityID entity_id) {
        if (I == 0) {
            m_entities[static_cast<int>(entity_id)] = false;
        }

        std::get<I>(m_pools).remove(entity_id);

        if constexpr (I + 1 != sizeof...(POOLS)) {
            destroy<I + 1>(entity_id);
        }
    }

    template <IsComponent... COMPONENTS>
    decltype(auto) get(EntityID entity_id) {
        return std::make_tuple(
            get_component_for_entity<COMPONENTS>(entity_id)...);
    }

    std::optional<EntityID> get_next_valid_entity(EntityID entity_id) {
        if (entity_id >= ENTITY_LIMIT - 1) {
            return std::nullopt;
        }

        do {
            entity_id++;
            if (m_entities[entity_id]) {
                return entity_id;
            }
        } while (entity_id < ENTITY_LIMIT - 1);

        return std::nullopt;
    }

    template <size_t I, IsComponent... COMPONENTS>
    bool entity_has_all_components(EntityID entity_id) {
        using COMPONENTS_TUPLE = std::tuple<COMPONENTS...>;
        using CURRENT_COM = get_tuple_type<I, COMPONENTS_TUPLE>::Type;

        constexpr int idx = get_pool_idx<CURRENT_COM>();
        auto* pool = &std::get<idx>(m_pools);
        if (!pool->has(entity_id)) {
            return false;
        }

        if constexpr (I + 1 != sizeof...(COMPONENTS)) {
            return entity_has_all_components<I + 1, COMPONENTS...>(entity_id);
        }

        return true;
    }

    template <IsComponent... COMPONENTS>
    bool entity_has_all_components(EntityID entity_id) {
        return entity_has_all_components<0, COMPONENTS...>(entity_id);
    }

    template <IsComponent... COMS>
    std::optional<std::tuple<COMS&...>> try_get(EntityID entity_id) {
        if (!entity_has_all_components<0, COMS...>(entity_id)) {
            return std::nullopt;
        }

        return std::make_tuple(get_component_for_entity<COMS>(entity_id)...);
    }

    template <IsComponent... COMPONENTS>
    class View {
       public:
        using FilledRegistry = Registry<ENTITY_LIMIT, POOLS...>;

       private:
        FilledRegistry& m_registry;
        std::optional<EntityID> m_current_top;

        void find_next_entity() {
            if (!m_current_top.has_value()) {
                return;
            }

            std::optional<EntityID> entity_id =
                m_registry.get_next_valid_entity(*m_current_top);
            if (!entity_id.has_value()) {
                m_current_top = std::nullopt;
                return;
            }

            bool has_all_coms =
                m_registry.template entity_has_all_components<COMPONENTS...>(
                    *entity_id);
            if (has_all_coms) {
                m_current_top = *entity_id;
                return;
            }

            find_next_entity();
        }

       public:
        explicit View(FilledRegistry& registry)
            : m_registry(registry), m_current_top(0) {}
        explicit View(FilledRegistry& registry, EntityID current_top)
            : m_registry(registry), m_current_top(current_top) {}

        std::tuple<COMPONENTS...> get() {
            return m_registry.template get<COMPONENTS...>(*m_current_top);
        }

        // Prefix increment
        View<COMPONENTS...>& operator++() {
            find_next_entity();
            return *this;
        }

        // Postfix increment
        View<COMPONENTS...> operator++(int) {
            View<COMPONENTS...> tmp = *this;
            ++(*this);
            return tmp;
        }

        bool complete() { return m_current_top == std::nullopt; }

        friend bool operator==(const View<COMPONENTS...>& a,
                               const View<COMPONENTS...>& b) {
            return a.m_current_top == b.m_current_top;
        };

        friend bool operator!=(const View<COMPONENTS...>& a,
                               const View<COMPONENTS...>& b) {
            return a.m_current_top != b.m_current_top;
        };
    };

    template <IsComponent... COMPONENTS>
    View<COMPONENTS...> view() {
        return View<COMPONENTS...>(*this);
    }
};

template <typename T, IsComponent... COMPONENTS>
class View {
   private:
    T& m_registry;
    std::optional<EntityID> m_current_top;

    void find_next_entity() {
        if (!m_current_top.has_value()) {
            return;
        }

        std::optional<EntityID> entity_id =
            m_registry.get_next_valid_entity(*m_current_top);
        if (!entity_id.has_value()) {
            m_current_top = std::nullopt;
            return;
        }

        bool has_all_coms =
            m_registry.template entity_has_all_components<COMPONENTS...>(
                *entity_id);
        if (has_all_coms) {
            m_current_top = *entity_id;
            return;
        }

        find_next_entity();
    }

   public:
    explicit View(T& registry) : m_registry(registry), m_current_top(0) {}
    explicit View(T& registry, EntityID current_top)
        : m_registry(registry), m_current_top(current_top) {}

    std::tuple<COMPONENTS...> get() {
        return m_registry.template get<COMPONENTS...>(*m_current_top);
    }

    // Prefix increment
    View<T, COMPONENTS...>& operator++() {
        find_next_entity();
        return *this;
    }

    // Postfix increment
    View<T, COMPONENTS...> operator++(int) {
        View<T, COMPONENTS...> tmp = *this;
        ++(*this);
        return tmp;
    }

    bool complete() { return m_current_top == std::nullopt; }

    friend bool operator==(const View<T, COMPONENTS...>& a,
                           const View<T, COMPONENTS...>& b) {
        return a.m_current_top == b.m_current_top;
    };

    friend bool operator!=(const View<T, COMPONENTS...>& a,
                           const View<T, COMPONENTS...>& b) {
        return a.m_current_top != b.m_current_top;
    };
};

}  // namespace mgm::ecs