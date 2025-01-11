//
// Created by mkowa on 11.01.2025.
//

#pragma once

#include <queue>
#include <memory>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <iostream>

namespace bilist {
    template<class T>
    class bilist;

/**
 * @brief Element listy dwukierunkowej
 * @tparam T typ elementu przechowanego przez element
 */
    template<class T>
    class bilistelement {
        friend class bilist<T>;

        T val;
        //Wskaźniki na poprzedni i kolejny element
        std::shared_ptr<bilistelement<T>> next;
        std::shared_ptr<bilistelement<T>> prev;
    public:
        // Konst. Bezarg.
        bilistelement() : next(nullptr), prev(nullptr), val(nullptr) {

        }

        //Konst. tworzący element z podaną wartością
        bilistelement(T v) : next(nullptr), prev(nullptr), val(v) {

        }
    };

    template<class T>
    class bilist {
    private:
        size_t m_size; // liczba elementow
        // wsk. na pierwszy element
        std::shared_ptr<bilistelement<T>> m_first;
        // wsk. na element ostatni
        std::shared_ptr<bilistelement<T>> m_last;
    public:
        /**
         * @brief iterator dla listy dwukierunkowej
         */
        struct iterator {
            using iterator_category = std::bidirectional_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using pointer = T *;
            using reference = T &;

            friend class bilist<T>;

            // Konstruktory
            iterator(std::shared_ptr<bilistelement<T>> ptr) : m_ptr(ptr) {

            };

            iterator(iterator &it) : m_ptr(it.m_ptr) {

            };

            iterator(iterator &&it) : m_ptr(it.m_ptr) {

            };

            reference operator*() const { return m_ptr->val; }

            pointer operator->() { return &m_ptr->val; }


            iterator &operator++() {
                m_ptr = m_ptr->next;
                return *this;
            }

            iterator &operator--() {
                m_ptr = m_ptr->prev;
                return *this;
            }

            iterator operator++(int) {
                iterator tmp = *this;
                ++(*this);
                return tmp;
            }

            iterator operator--(int) {
                iterator tmp = *this;
                --(*this);
                return tmp;
            }

            friend bool operator==(const iterator &a, const iterator &b) { return a.m_ptr == b.m_ptr; };

            friend bool operator!=(const iterator &a, const iterator &b) { return a.m_ptr != b.m_ptr; };
        private:
            std::shared_ptr<bilistelement<T>> m_ptr;
        };

        // konstruktor bezargumentowy listy
        bilist() : m_size(0), m_first(nullptr), m_last(nullptr) {

        }

        // konst. kopiujący
        bilist(bilist &t) = default;

        // konst. przenoszący
        bilist(bilist &&obj) {
            m_size = obj.size();
            m_first = obj.m_first;
            m_last = obj.m_last;

            obj.m_size = 0;
            obj.m_first = nullptr;
            obj.m_last = nullptr;
        }

        size_t size() {
            return m_size;
        }

        // dodaj na koniec
        void push_back(T val) {
            std::shared_ptr<bilistelement<T>> tmp(new bilistelement<T>(val));

            if (m_size == 0) {
                m_first = tmp;
                m_last = tmp;
                m_size++;
                return;
            }
            //bilistelement<T> *cur = m_last;
            std::shared_ptr<bilistelement<T>> cur = m_last;
            cur->next = tmp;
            tmp->prev = cur;
            m_last = tmp;
            m_size++;

        }

        // dodaj z przodu
        void push_front(T val) {
            std::shared_ptr<bilistelement<T>> tmp(new bilistelement<T>(val));

            if (m_size == 0) {
                m_first = tmp;
                m_last = tmp;
                m_size++;
                return;
            }
            //bilistelement<T> *cur = m_first;
            std::shared_ptr<bilistelement<T>> cur = m_first;
            cur->prev = tmp;
            tmp->next = cur;
            m_first = tmp;
            m_size++;
        }

        // usuń z końca
        void pop_back() {
            if (m_size < 0) throw std::runtime_error("ERR: Proba wywolania pop_back() na pustej liscie!");
            m_size--;
            if (m_size == 0) {
                m_last.reset();
                m_last = nullptr;
                m_first = nullptr;
                return;
            }
            auto tmp = m_last->prev;
            m_last.reset();
            tmp->next = nullptr;
            m_last = tmp;
        }

        // usuń z przodu
        void pop_front() {
            if (m_size < 0) throw std::runtime_error("ERR: Proba wywolania pop_front() na pustej liscie!");
            m_size--;
            if (m_size == 0) {
                m_last.reset();
                m_last = nullptr;
                m_first = nullptr;
                return;
            }
            auto tmp = m_first->next;
            m_first.reset();
            tmp->prev = nullptr;
            m_first = tmp;

        }

        const T &front() const {
            return m_first->val;
        }

        T &front() {
            return m_first->val;
        }

        const T &back() const {
            return m_last->val;
        }

        T &back() {
            return m_last->val;
        }

        T &at(size_t index) {
            if (index >= m_size or index < 0)
                throw std::runtime_error("ERR: Proba odczytania elementu na pozycji spoza listy!");
            iterator a = m_first;
            for (int i = 0; i < index; a++, i++) {

            }
            return *a;
        }

        bool empty() const {
            return m_size == 0;
        }

        void clear() {
            while (!empty()) {
                pop_back();
            }
        }

        iterator begin() {
            return iterator(m_first);
        }

        iterator end() {
            return iterator(nullptr);
        }

        // copy assignment operator
        bilist &operator=(bilist &other) = default;

        // move assignment operator
        bilist &operator=(bilist &&other) {
            if (this != &other) {
                m_size = other.size();
                m_first = other.m_first;
                m_last = other.m_last;

                other.m_first = nullptr;
                other.m_last = nullptr;
                other.m_size = 0;
            }
            return *this;
        };

        // wyszukiwanie elementu o podanej wartosci
        iterator find(T y) {
            iterator x = this->begin();
            x++;
            for (; x != this->end(); x++) {
                if (*x == y) break;
            }
            return x;
        }

        // bubble sort
        void sort() {
            for (int i = 0; i < m_size - 1; ++i) {
                for (int j = 0; j + i < m_size - 1; ++j) {
                    if (this->at(j) > this->at(j + 1)) {
                        std::swap(this->at(j), this->at(j + 1));
                    }
                }
            }
        }

        void serialize(const std::string &filename) {
            std::ofstream file(filename, std::ios::binary);
            if (!file.is_open()) {
                std::cerr
                        << "ERR: Bład otwierania pliku " << filename << " do zapisu!"
                        << std::endl;
                return;
            }
            file.write(reinterpret_cast<const char *>(this),
                       sizeof(*this));
            file.close();
        }

        static bilist deserialize(const std::string &filename) {
            bilist obj;
            std::ifstream file(filename, std::ios::binary);
            if (!file.is_open()) {
                std::cerr
                        << "ERR: Blad otwierania pliku " << filename << " do odczytu!"
                        << std::endl;
                return obj;
            }
            file.read(reinterpret_cast<char *>(&obj),
                      sizeof(obj));
            file.close();
            return obj;
        }
    };

    template<typename typZmiennej>
    class queue_adaptor {
        bilist<typZmiennej> l;
    public:
        typedef typZmiennej value_type;
        typedef typZmiennej &reference;
        typedef const typZmiennej &const_reference;
        typedef size_t size_type;

        void push_back(const_reference zmienna) {
            l.push_back(zmienna);
        }

        void pop_front(void) { l.pop_front(); }

        bool empty(void) const { return l.empty(); }

        size_type size(void) const { return l.size(); }

        reference front(void) { return l.front(); }

        reference back(void) { return l.back(); }

        queue_adaptor() {}

        queue_adaptor(const queue_adaptor &skopiuj) {}

        ~queue_adaptor(void) {}
    };
}