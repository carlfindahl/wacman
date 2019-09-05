#pragma once

#include <vector>
#include <memory>

namespace pac
{
/*!
 * \brief Represents various exit codes for the behaviour tree
 */
enum class EBehaviourStatus
{
    Invalid,    // Tree is not in a good place
    Running,    // Behaviour is still going
    Finished,   // Success State
    Suspended,  // Waiting State
    Failed      // Fail State
};

/*!
 * \brief Base class for behaviour tree behaviours (leaf nodes)
 */
class behaviour
{
private:
    /** Current status */
    EBehaviourStatus m_status = EBehaviourStatus::Invalid;

protected:
    /*!
     * \brief on_initialize is called the first time before the node is updated
     */
    virtual void on_initialize() = 0;

    /*!
     * \brief update called once every time the tree is updated
     * \param dt delta time
     * \return a status code
     */
    virtual EBehaviourStatus update() = 0;

    /*!
     * \brief on_terminate is called is called immediately after update returns a status
     */
    virtual void on_terminate() = 0;

public:
    /*!
     * \brief wrapper to avoid basic bt mistakes
     * \return status of behaviour
     */
    EBehaviourStatus tick();
};

/*!
 * \brief Decorators wrap behaviours and decorate them with various functionality
 */
class decorator : public behaviour
{
protected:
    /** Decorated behaviour */
    std::unique_ptr<behaviour> m_child = nullptr;

public:
    decorator(std::unique_ptr<behaviour> child) : m_child(std::move(child)) {}
};

class composite : public behaviour
{
protected:
    std::vector<std::unique_ptr<behaviour>> m_children{};
};

/*!
 * \brief Wraps the functionality of a behaviour tree
 */
class behaviour_tree
{
private:
    /** Root node in B-tree */
    std::unique_ptr<behaviour> m_root{};

public:
    /*!
     * \brief update tree (traverse nodes etc.)
     */
    void update();
};

}  // namespace pac
