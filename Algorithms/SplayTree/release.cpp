#include <iostream>
#include <algorithm>
#include <tuple>

template<typename T>
class SplayTree {
private:

    struct Node {
        size_t size;
        T value;
        T sum;

        bool isReversed;
        bool isAssigned;
        T add;

        size_t maxNonIncreasingSuffix;
        size_t maxNonDecreasingPrefix;
        size_t maxNonDecreasingSuffix;
        size_t maxNonIncreasingPrefix;
        T leftest;
        T rightest;

        Node* left;
        Node* right;
        Node* parent;

        Node(T value)
                : size(1), value(value), sum(value), isReversed(false),
                  isAssigned(false), add(0),
                  maxNonIncreasingSuffix(1), maxNonDecreasingPrefix(1),
                  maxNonDecreasingSuffix(1), maxNonIncreasingPrefix(1),
                  leftest(value), rightest(value),
                  left(nullptr), right(nullptr), parent(nullptr)
        {   }
    };

    Node* root = nullptr;


    static size_t getSize(Node* node) {
        if (node == nullptr)
            return 0;
        return node->size;
    }

    static T getSum(Node* node) {
        if (node == nullptr)
            return 0;
        return node->sum;
    }

    static size_t getMaxNonIncreasingSuffix(Node* node) {
        if (node == nullptr)
            return 0;
        return node->maxNonIncreasingSuffix;
    }

    static size_t getMaxNonDecreasingPrefix(Node* node) {
        if (node == nullptr)
            return 0;
        return node->maxNonDecreasingPrefix;
    }

    static size_t getMaxNonDecreasingSuffix(Node* node) {
        if (node == nullptr)
            return 0;
        return node->maxNonDecreasingSuffix;
    }

    static size_t getMaxNonIncreasingPrefix(Node* node) {
        if (node == nullptr)
            return 0;
        return node->maxNonIncreasingPrefix;
    }





    static void toggleReversed(Node* node) {
        if (node != nullptr)
            node->isReversed ^= true;
    }

    static void lazyAdd(Node* node, T value) {
        if (node != nullptr)
            node->add += value;
    }

    static void setAssigned(Node* node) {
        if (node != nullptr) {
            node->isAssigned = true;
            node->add = 0;
        }
    }

    static void push(Node* node) {
        if (node == nullptr)
            return;
        if (node->isReversed) {
            node->isReversed = false;
            std::swap(node->maxNonIncreasingSuffix, node->maxNonDecreasingPrefix);
            std::swap(node->maxNonDecreasingSuffix, node->maxNonIncreasingPrefix);
            std::swap(node->leftest, node->rightest);
            std::swap(node->left, node->right);
            toggleReversed(node->left);
            toggleReversed(node->right);
        }

        if (node->isAssigned) {
            node->value = 0;
            node->sum = 0;

            node->maxNonIncreasingSuffix = node->size;
            node->maxNonDecreasingPrefix = node->size;
            node->maxNonDecreasingSuffix = node->size;
            node->maxNonIncreasingPrefix = node->size;

            node->leftest = 0;
            node->rightest = 0;

            setAssigned(node->left);
            setAssigned(node->right);

            node->isAssigned = false;
        }

        if (node->add != 0) {
            node->value += node->add;
            node->sum += node->size * node->add;

            node->leftest += node->add;
            node->rightest += node->add;

            lazyAdd(node->left, node->add);
            lazyAdd(node->right, node->add);

            node->add = 0;
        }
    }


    static void update(Node* node) {
        if (node == nullptr)
            return;

        push(node);
        push(node->left);
        push(node->right);

        node->size = getSize(node->left) + getSize(node->right) + 1;
        node->sum = getSum(node->left) + getSum(node->right) + node->value;

        if (node->left == nullptr)
            node->leftest = node->value;
        else
            node->leftest = node->left->leftest;

        if (node->right == nullptr)
            node->rightest = node->value;
        else
            node->rightest = node->right->rightest;



        node->maxNonIncreasingSuffix = getMaxNonIncreasingSuffix(node->right);

        if (getMaxNonIncreasingSuffix(node->right) == getSize(node->right)) {
            if (node->right == nullptr || node->value >= node->right->leftest) {
                ++node->maxNonIncreasingSuffix;

                if (node->left != nullptr && node->left->rightest >= node->value)
                    node->maxNonIncreasingSuffix += node->left->maxNonIncreasingSuffix;
            }
        }



        node->maxNonDecreasingPrefix = getMaxNonDecreasingPrefix(node->left);

        if (getMaxNonDecreasingPrefix(node->left) == getSize(node->left)) {
            if (node->left == nullptr || node->value >= node->left->rightest) {
                ++node->maxNonDecreasingPrefix;

                if (node->right != nullptr && node->right->leftest >= node->value)
                    node->maxNonDecreasingPrefix += node->right->maxNonDecreasingPrefix;
            }
        }




        node->maxNonDecreasingSuffix = getMaxNonDecreasingSuffix(node->right);

        if (getMaxNonDecreasingSuffix(node->right) == getSize(node->right)) {
            if (node->right == nullptr || node->value <= node->right->leftest) {
                ++node->maxNonDecreasingSuffix;

                if (node->left != nullptr && node->left->rightest <= node->value)
                    node->maxNonDecreasingSuffix += node->left->maxNonDecreasingSuffix;
            }
        }



        node->maxNonIncreasingPrefix = getMaxNonIncreasingPrefix(node->left);

        if (getMaxNonIncreasingPrefix(node->left) == getSize(node->left)) {
            if (node->left == nullptr || node->value <= node->left->rightest) {
                ++node->maxNonIncreasingPrefix;

                if (node->right != nullptr && node->right->leftest <= node->value)
                    node->maxNonIncreasingPrefix += node->right->maxNonIncreasingPrefix;
            }
        }
    }

    static void setParent(Node* child, Node* parent) {
        if (child != nullptr)
            child->parent = parent;
    }

    static void leftRotation(Node* p) {
        Node* x = p->right;
        Node* b = x->left;
        Node* c = x->right;
        Node* a = p->left;
        Node* g = p->parent;

        setParent(b, p);
        p->right = b;

        setParent(p, x);
        x->left = p;

        setParent(x, g);

        if (g != nullptr) {
            if (p == g->left)
                g->left = x;
            else
                g->right = x;
        }

        update(p);
        update(x);
        update(g);
    }

    static void rightRotation(Node* p) {
        Node* x = p->left;
        Node* a = x->left;
        Node* b = x->right;
        Node* c = p->right;
        Node* g = p->parent;

        setParent(b, p);
        p->left = b;

        setParent(p, x);
        x->right = p;

        setParent(x, g);

        if (g != nullptr) {
            if (p == g->left)
                g->left = x;
            else
                g->right = x;
        }

        update(p);
        update(x);
        update(g);
    }


    static void leftZig(Node* node) {
        rightRotation(node->parent);
    }

    static void rightZig(Node* node) {
        leftRotation(node->parent);
    }

    static void leftZigZig(Node* node) {
        rightRotation(node->parent->parent);
        rightRotation(node->parent);
    }

    static void rightZigZig(Node* node) {
        leftRotation(node->parent->parent);
        leftRotation(node->parent);
    }

    static void leftZigZag(Node* node) {
        rightRotation(node->parent);
        leftRotation(node->parent);
    }

    static void rightZigZag(Node* node) {
        leftRotation(node->parent);
        rightRotation(node->parent);
    }


    static void splay(Node* node) {
        if (node == nullptr || node->parent == nullptr)
            return;

        if (node->parent->parent == nullptr) {
            if (node == node->parent->left)
                leftZig(node);
            else
                rightZig(node);
        } else {
            if (node == node->parent->left) {
                if (node->parent == node->parent->parent->left)
                    leftZigZig(node);
                else
                    leftZigZag(node);
            } else {
                if (node->parent == node->parent->parent->right)
                    rightZigZig(node);
                else
                    rightZigZag(node);
            }
            splay(node);
        }
    }

    static Node* innerGet(Node* node, size_t index) {
        if (node == nullptr)
            return nullptr;

        push(node);
        push(node->left);
        push(node->right);

        if (getSize(node->left) == index) {
            splay(node);
            return node;
        } else if (getSize(node->left) > index) {
            return innerGet(node->left, index);
        } else {
            return innerGet(node->right, index - getSize(node->left) - 1);
        }
    }


    static Node* merge(Node* left, Node* right) {
        push(left);
        push(right);

        if (left == nullptr)
            return right;
        if (right == nullptr)
            return left;

        Node* newRoot = innerGet(right, 0);

        setParent(left, newRoot);
        newRoot->left = left;

        update(newRoot);

        return newRoot;
    }

    static std::pair<Node*, Node*> split(Node* tree, size_t index) {
        if (tree == nullptr)
            return std::make_pair(nullptr, nullptr);

        push(tree);

        if (index == -1)
            return std::make_pair(nullptr, tree);

        Node* left = innerGet(tree, index);
        Node* right = left->right;

        setParent(right, nullptr);
        left->right = nullptr;
        update(left);

        return std::make_pair(left, right);
    }

    static Node* innerInsert(Node* tree, Node* x, size_t index) {
        Node* left;
        Node* right;

        std::tie(left, right) = split(tree, index - 1);
        setParent(left, x);
        setParent(right, x);

        x->left = left;
        x->right = right;

        update(x);

        return x;
    }

    static Node* removeRoot(Node* x) {
        push(x);

        Node* left = x->left;
        Node* right = x->right;

        setParent(left, nullptr);
        setParent(right, nullptr);

        delete x;

        return merge(left, right);
    }

    static std::tuple<Node*, Node*, Node*> splitOutSubsegment(Node* tree, size_t l, size_t r) {
        Node* left;
        Node* mid;
        Node* right;

        std::tie(left, right) = split(tree, l - 1);
        std::tie(mid, right) = split(right, r - l);

        return std::make_tuple(left, mid, right);
    }

    static Node* splitInSubsegment(Node* left, Node* mid, Node* right) {
        left = merge(left, mid);
        return merge(left, right);
    }

    static std::pair<Node*, T> innerRsq(Node* tree, size_t l, size_t r) {
        Node* left;
        Node* mid;
        Node* right;

        std::tie(left, mid, right) = splitOutSubsegment(tree, l, r);

        T result = getSum(mid);

        return std::make_pair(splitInSubsegment(left, mid, right), result);
    }

    static Node* innerAssign(Node* tree, size_t l, size_t r, T value) {
        Node* left;
        Node* mid;
        Node* right;

        std::tie(left, mid, right) = splitOutSubsegment(tree, l, r);

        setAssigned(mid);
        lazyAdd(mid, value);

        return splitInSubsegment(left, mid, right);
    }

    static Node* innerAdd(Node* tree, size_t l, size_t r, size_t value) {
        Node* left;
        Node* mid;
        Node* right;

        std::tie(left, mid, right) = splitOutSubsegment(tree, l, r);

        lazyAdd(mid, value);

        return splitInSubsegment(left, mid, right);
    }

    static Node* innerMajOnNonIncreasing(Node* node, T value) {
        if (node == nullptr)
            return nullptr;

        push(node);
        push(node->left);
        push(node->right);

        if (node->value <= value) {
            return innerMajOnNonIncreasing(node->left, value);
        } else if (node->right == nullptr || node->right->leftest <= value) {
            splay(node);
            return node;
        } else {
            return innerMajOnNonIncreasing(node->right, value);
        }
    }

    static Node* innerMinOnNonDecreasing(Node* node, T value) {
        if (node == nullptr)
            return nullptr;

        push(node);
        push(node->left);
        push(node->right);

        if (node->value >= value) {
            return innerMinOnNonDecreasing(node->left, value);
        } else if (node->right == nullptr || node->right->leftest >= value) {
            splay(node);
            return node;
        } else {
            return innerMinOnNonDecreasing(node->right, value);
        }
    }

    static Node* innerNextPermutation(Node* tree, size_t l, size_t r) {
        if (tree == nullptr)
            return nullptr;

        Node* left;
        Node* mid;
        Node* right;

        std::tie(left, mid, right) = splitOutSubsegment(tree, l, r);

        size_t suffixSize = getMaxNonIncreasingSuffix(mid);

        if (suffixSize == getSize(mid)) {
            toggleReversed(mid);
            return splitInSubsegment(left, mid, right);
        }

        Node* suffix;

        std::tie(mid, suffix) = split(mid, getSize(mid) - suffixSize - 1);
        suffix = innerMajOnNonIncreasing(suffix, mid->value);

        std::swap(mid->value, suffix->value);
        update(mid);
        update(suffix);
        toggleReversed(suffix);

        mid = merge(mid, suffix);

        return splitInSubsegment(left, mid, right);
    }

    static Node* innerPrevPermutation(Node* tree, size_t l, size_t r) {
        if (tree == nullptr)
            return nullptr;

        Node* left;
        Node* mid;
        Node* right;

        std::tie(left, mid, right) = splitOutSubsegment(tree, l, r);

        size_t suffixSize = getMaxNonDecreasingSuffix(mid);

        if (suffixSize == getSize(mid)) {
            toggleReversed(mid);
            return splitInSubsegment(left, mid, right);
        }

        Node* suffix;

        std::tie(mid, suffix) = split(mid, getSize(mid) - suffixSize - 1);
        suffix = innerMinOnNonDecreasing(suffix, mid->value);

        std::swap(mid->value, suffix->value);
        update(mid);
        update(suffix);
        toggleReversed(suffix);

        mid = merge(mid, suffix);

        return splitInSubsegment(left, mid, right);
    }

public:
    SplayTree() = default;

    ~SplayTree() {
        while (root != nullptr) {
            root = removeRoot(root);
        }
    }

    void append(T x) {
        Node* newNode = new Node(x);
        root = merge(root, newNode);
    }

    T rsq(size_t l, size_t r) {
        T sum;
        std::tie(root, sum) = innerRsq(root, l, r);

        return sum;
    }

    void insert(size_t pos, T value) {
        Node* newNode = new Node(value);
        root = innerInsert(root, newNode, pos);
    }

    void remove(size_t pos) {
        root = innerGet(root, pos);
        root = removeRoot(root);
    }

    void assign(size_t l, size_t r, T value) {
        root = innerAssign(root, l, r, value);
    }

    void add(size_t l, size_t r, T value) {
        root = innerAdd(root, l, r, value);
    }

    void nextPermutation(size_t l, size_t r) {
        root = innerNextPermutation(root, l, r);
    }

    void prevPermutation(size_t l, size_t r) {
        root = innerPrevPermutation(root, l, r);
    }

    T get(int index) {
        root = innerGet(root, index);
        return root->value;
    }

    [[nodiscard]] size_t size() const {
        return getSize(root);
    }

};



int main() {
    std::ios_base::sync_with_stdio(false);

    size_t n;
    std::cin >> n;

    SplayTree<int64_t> tree;

    size_t counter = 0;
    for (counter = 1; counter <= n; ++counter) {
        int64_t x;
        std::cin >> x;

        tree.append(x);
    }

    size_t q;
    std::cin >> q;

    while (q--) {
        size_t com;
        std::cin >> com;

        switch(com) {
            case 1: {
                size_t l;
                size_t r;
                std::cin >> l >> r;

                std::cout << tree.rsq(l, r) << '\n';

                break;
            }
            case 2: {
                int64_t value;
                size_t pos;
                std::cin >> value >> pos;

                tree.insert(pos, value);

                break;
            }
            case 3: {
                size_t pos;
                std::cin >> pos;

                tree.remove(pos);

                break;
            }
            case 4: {
                int64_t value;
                size_t l;
                size_t r;

                std::cin >> value >> l >> r;

                tree.assign(l, r, value);

                break;
            }
            case 5: {
                int64_t value;
                size_t l;
                size_t r;

                std::cin >> value >> l >> r;

                tree.add(l, r, value);

                break;
            }
            case 6: {
                size_t l;
                size_t r;

                std::cin >> l >> r;

                tree.nextPermutation(l, r);

                break;
            }
            case 7: {
                size_t l;
                size_t r;

                std::cin >> l >> r;

                tree.prevPermutation(l, r);

                break;
            }
            default: {}
        }
    }

    for (size_t i = 0; i < tree.size(); ++i) {
        std::cout << tree.get(i) << ' ';
    }
    std::cout << '\n';

    return 0;
}
