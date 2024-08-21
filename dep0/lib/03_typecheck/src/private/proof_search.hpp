#pragma once

#include "dep0/typecheck/ast.hpp"
#include "dep0/typecheck/context.hpp"
#include "dep0/typecheck/environment.hpp"

#include "private/usage.hpp"

#include <functional>
#include <optional>

namespace dep0::typecheck {

/**
 * Opaque data structure to store the current state of a proof search.
 * @see search_task_t
 */
struct search_state_t;

/**
 * In complex type theories, like one with dependent types, proof-search is undecidable.
 * Because of this, a naive depth-first search may easily get stuck.
 * Even if it succeeds, it may follow a very long path and generate an unnecessary complex result.
 * Instead we implement a breadth-first search.
 * Each branch of the search space is assigned to single a task, so that:
 *   1. if a branch is stuck in a loop, it will not prevent other tasks from progressing
 *   2. the task following the shortest path (i.e. the simplest expression) will win.
 * Typically a task starts out by trying only one individual tactic.
 * Some tactics are terminal, meaning that they either succeed or fail.
 * Other tactics require to spawn many sub-tasks in either of 2 fashions:
 *   1. if any sub-task succeeds, the overall task succeeds, typically because any alternative is equally valid;
 *   2. all sub-tasks must succeed for the overall task to succeed and their results must be combined in some way.
 */
class search_task_t
{
    /**
     * A task of this kind will apply one individual tactic.
     * If the tactic needs to spawn some sub-tasks, it will do so
     * by invoking either `when_all` or `when_any`, thereby
     * changing the current task kind accordingly.
     * Otherwise, the tactic in this task will either succeed or fail.
     * Success is marked explicitly by calling `set_result`.
     * Failure can be either explicit or implicit, by calling `set_failed` or not.
     */
    struct one_t
    {
        std::function<void(search_task_t&)> tactic;
    };

    /**
     * A task of this kind will succeed if any sub-task succeeds.
     * The result of this task is the result of the sub-task that succeeded.
     * If all sub-tasks fail, this task fails.
     */
    struct any_t
    {
        std::vector<search_task_t> sub_tasks;
    };

    /**
     * A task of this kind will succeed only if all sub-tasks succeed.
     * The result of this task is produced by calling `build_result` with the results of all sub-tasks.
     * If any sub-task fails, this task will fail.
     * A temporary usage object is shared by all sub-tasks;
     * if all sub-tasks succeed the total usage from all sub-tasks is added to the main usage count;
     * otherwise the usage count from any partial success is discarded without affecting the main usage count.
     */
    struct all_t
    {
        std::vector<search_task_t> sub_tasks;
        std::shared_ptr<usage_t> temp_usage;
        std::function<expr_t(std::vector<expr_t>)> build_result;
    };

    /**
     * Tag that indicates that the current task is still in progress.
     * A task may get stuck and remain forever in progress.
     */
    struct in_progress_t { };

    /** Tag that indicates that the current task has failed. */
    struct failed_t { };

    /** The task has succeeded and its result is stored here. */
    struct succeeded_t { expr_t result; };

    std::variant<one_t, any_t, all_t> m_kind;
    std::variant<in_progress_t, failed_t, succeeded_t> m_status;

public:
    search_state_t& state;
    std::size_t const depth; /**< Level of depth in the search path; if too deep this task will fail. */
    env_t const& env;
    ctx_t const& ctx;
    std::shared_ptr<expr_t const> const target; // TODO use some `shared_ref` since can never be nullptr or `type_t`
    ast::is_mutable_t const is_mutable_allowed;
    ast::qty_t const usage_multiplier;
    std::shared_ptr<usage_t> usage;

    search_task_t(
        search_state_t&,
        std::size_t depth,
        std::shared_ptr<expr_t const> target,
        ast::is_mutable_t is_mutable_allowed,
        std::shared_ptr<usage_t>,
        ast::qty_t usage_multiplier,
        std::function<void(search_task_t&)>);

    bool done() const;      /**< True if this task has finished, either failed or succeeded. */
    bool failed() const;    /**< True if this task has failed. */
    bool succeeded() const; /**< True if this task has succeeded. */

    /**
     * If the task succeeded, returns its result; undefined behaviour otherwise.
     * @{
     */
    expr_t& result();
    expr_t const& result() const;
    /** @} */

    void set_failed();
    void set_result(expr_t);

    /**
     * Try to make some progress, if this task has not yet finished.
     * When this function returns, the task may still not have finished.
     * For example if it's still waiting for some sub-task to finish.
     */
    void run();

    /**
     * Transform the current task into one that awaits all of the given sub-tasks.
     *
     * @param temp_usage
     *      All sub-tasks must share the same usage object.
     *      If any sub-task fails the usage from a partial success is discarded.
     *      If all sub-tasks succeed, their total usage is added to the usage count of the parent task.
     *
     * @param build_result
     *      If all sub-tasks succeed, their result is passed to this function to produce the final result of the task.
     */
    void when_all(
        std::vector<search_task_t>,
        std::shared_ptr<usage_t> temp_usage,
        std::function<expr_t(std::vector<expr_t>)> build_result);

    /**
     * Transform the current task into one that awaits any of the given sub-tasks.
     * The result of this task is the result of the first task that succeeds.
     */
    void when_any(std::vector<search_task_t>);
};

/**
 * Search for a value of the given type in the given environment and context.
 * If none can be found, returns an empty optional.
 * A term is only viable if its use does not exceed what is allowed by the context.
 * For example, if the given context contains a variable of the desired type,
 * but its quantity is 0, it will only be used if the multiplier is also 0.
 * Similary if its quantity is 1 but it has already been used, then it will not be viable.
 * If no term could be found, the input usage is guaranteed to be unchanged.
 *
 * @param is_mutable_allowed
 *      Specifies whether mutable functions are viable proof terms.
 *
 * @param usage,usage_multiplier
 *      @see usage_t
 */
std::optional<expr_t>
search_proof(
    env_t const&,
    ctx_t const&,
    expr_t const& type,
    ast::is_mutable_t is_mutable_allowed,
    usage_t& usage,
    ast::qty_t usage_multiplier);

/**
 * A meta-tactic that applies all known proof-search tactics.
 * It is usually the entry point for a brand new search task.
 * Use it if you need to spawn a sub-task to search for some intermdiate proof.
 */
void proof_search(search_task_t&);

} // namespace dep0::typecheck
