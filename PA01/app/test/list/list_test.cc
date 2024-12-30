/**
 * @file list_test.cc
 * @author WittXie
 * @brief 链表测试
 * @version 0.1
 * @date 2024-09-19
 * @note
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "./../test_app.h"
// #include "./list.cc"

typedef struct
{
    int id;
    char name[20];
} test_data_t;

// 比较函数，用于排序和查找
int8_t compare_by_id(list_node_t *node1, list_node_t *node2)
{
    test_data_t *data1 = (test_data_t *)node1->data;
    test_data_t *data2 = (test_data_t *)node2->data;
    return (data1->id - data2->id);
}

// 匹配函数，用于查找
int8_t match_by_id(list_node_t *node)
{
    test_data_t *data = (test_data_t *)node->data;
    return (data->id == 2) ? 1 : 0;
}

// 回调函数，用于遍历
void dprint_node(list_node_t *node)
{
    test_data_t *data = (test_data_t *)node->data;
    dprint("ID: %d, Name: %s\r\n", data->id, data->name);
}

static void list_test(void)
{
    dprint(COLOR_H_WHITE);
    dprint("Normal test for list start\r\n\n");
    // 初始化链表
    list_t list = {0};
    list_init(&list);

    // 创建测试数据
    test_data_t data1 = {1, "Alice"};
    test_data_t data2 = {2, "Bob"};
    test_data_t data3 = {3, "Charlie"};

    // 创建节点并插入链表
    list_node_t *node1 = list_node_create(&data1);
    list_node_t *node2 = list_node_create(&data2);
    list_node_t *node3 = list_node_create(&data3);

    list_insert_tail(&list, node1);
    list_insert_tail(&list, node2);
    list_insert_tail(&list, node3);

    // 验证链表长度
    ASSERT(list_length_get(&list) == 3, "Expected length: 3, Actual length: %d", list_length_get(&list));

    // 遍历链表并打印节点
    dprint("Initial list:\r\n");
    list_traverse(&list, dprint_node);

    // 查找节点
    list_node_t *found_node = list_find_node(&list, &data2);
    ASSERT(found_node != NULL, "Expected to find node with ID: 2, but found NULL");
    dprint("Found node with ID: %d\r\n", ((test_data_t *)found_node->data)->id);

    // 按匹配函数查找节点
    found_node = list_find_match_node(&list, match_by_id);
    ASSERT(found_node != NULL, "Expected to find node with ID: 2, but found NULL");
    dprint("Found node with ID: %d\r\n", ((test_data_t *)found_node->data)->id);

    // 删除节点
    list_destroy(&list, node2);
    ASSERT(list_length_get(&list) == 2, "Expected length: 2, Actual length: %d", list_length_get(&list));

    // 遍历链表并打印节点
    dprint("List after removal:\r\n");
    list_traverse(&list, dprint_node);

    // 插入新节点并排序
    test_data_t data4 = {4, "David"};
    list_node_t *node4 = list_node_create(&data4);
    list_compare_insert(&list, compare_by_id, node4);

    // 遍历链表并打印节点
    dprint("List after sorted insertion:\r\n");
    list_traverse(&list, dprint_node);

    // 销毁链表
    list_deinit(&list);

    dprint("Normal test for list passed!\r\n");

    // 边缘测试：单节点链表操作
    dprint("Edge test for single node list start\r\n\n");
    // 初始化链表
    list_t single_node_list = {0};
    list_init(&single_node_list);

    // 创建测试数据
    test_data_t single_data = {1, "Alice"};
    list_node_t *single_node = list_node_create(&single_data);

    // 插入单个节点
    list_insert_tail(&single_node_list, single_node);

    // 验证链表长度
    ASSERT(list_length_get(&single_node_list) == 1, "Expected length: 1, Actual length: %d", list_length_get(&single_node_list));

    // 遍历链表并打印节点
    dprint("Single node list:\r\n");
    list_traverse(&single_node_list, dprint_node);

    // 查找节点
    list_node_t *single_found_node = list_find_node(&single_node_list, &single_data);
    ASSERT(single_found_node != NULL, "Expected to find node with ID: 1, but found NULL");

    // 删除节点
    list_destroy(&single_node_list, single_node);
    ASSERT(list_length_get(&single_node_list) == 0, "Expected length: 0, Actual length: %d", list_length_get(&single_node_list));

    // 销毁链表
    list_deinit(&single_node_list);

    dprint("Edge test for single node list passed!\r\n");

    // 压力测试：插入大量节点
    dprint("Stress test for inserting nodes start\r\n\n");
    // 初始化链表
    list_t stress_list = {0};
    list_init(&stress_list);

    const int num_nodes = 1000;

    // 插入大量节点
    for (int i = 0; i < num_nodes; i++)
    {
        test_data_t data = {i, "Node"};
        list_node_t *node = list_node_create(&data);
        list_insert_tail(&stress_list, node);
    }

    // 验证链表长度
    ASSERT(list_length_get(&stress_list) == num_nodes, "Expected length: %d, Actual length: %d", num_nodes, list_length_get(&stress_list));

    // 销毁链表
    list_deinit(&stress_list);

    dprint("Stress test for inserting nodes passed!\r\n");

    // 压力测试：删除大量节点
    dprint("Stress test for removing nodes start\r\n\n");
    // 初始化链表
    list_t destroy_stress_list = {0};
    list_init(&destroy_stress_list);

    list_node_t *nodes[num_nodes];

    // 插入大量节点
    for (int i = 0; i < num_nodes; i++)
    {
        test_data_t data = {i, "Node"};
        nodes[i] = list_node_create(&data);
        list_insert_tail(&destroy_stress_list, nodes[i]);
    }

    // 验证链表长度
    ASSERT(list_length_get(&destroy_stress_list) == num_nodes, "Expected length: %d, Actual length: %d", num_nodes, list_length_get(&destroy_stress_list));

    // 删除大量节点
    list_destroy_all(&destroy_stress_list);

    // 验证链表长度
    ASSERT(list_length_get(&destroy_stress_list) == 0, "Expected length: 0, Actual length: %d", list_length_get(&destroy_stress_list));
    ASSERT(destroy_stress_list.head == NULL, "Expected head: NULL, Actual head: %p", destroy_stress_list.head);
    ASSERT(destroy_stress_list.tail == NULL, "Expected tail: NULL, Actual tail: %p", destroy_stress_list.tail);

    // 销毁链表
    list_deinit(&destroy_stress_list);

    dprint("Stress test for removing nodes passed!\r\n");

    // 测试：每次出表一半再入表一半（反复3次）
    dprint("Test for removing and inserting half nodes (repeated 3 times) start\r\n\n");
    list_t half_list = {0};
    list_init(&half_list);

    // 插入大量节点
    for (int i = 0; i < num_nodes; i++)
    {
        test_data_t data = {i, "Node"};
        list_insert_tail(&half_list, list_node_create(&data));
    }

    // 验证链表长度
    ASSERT(list_length_get(&half_list) == num_nodes, "Expected length: %d, Actual length: %d", num_nodes, list_length_get(&half_list));

    // 反复出表一半再入表一半
    for (int i = 0; i < 3; i++)
    {
        // 出表一半
        for (int j = 0; j < num_nodes / 2; j++)
        {
            list_destroy(&half_list, list_get_head(&half_list));
        }

        // 验证链表长度
        ASSERT(list_length_get(&half_list) == num_nodes / 2, "Expected length: %d, Actual length: %d", num_nodes / 2, list_length_get(&half_list));

        // 入表一半
        for (int j = 0; j < num_nodes / 2; j++)
        {
            test_data_t data = {i, "Node"};
            list_insert_tail(&half_list, list_node_create(&data));
        }

        // 验证链表长度
        ASSERT(list_length_get(&half_list) == num_nodes, "Expected length: %d, Actual length: %d", num_nodes, list_length_get(&half_list));
    }

    // 销毁链表
    list_deinit(&half_list);

    dprint("Test for removing and inserting half nodes (repeated 3 times) passed!\r\n");

    // 测试：每次全部出表再全部入表（反复3次）
    dprint("Test for removing and inserting all nodes (repeated 3 times) start\r\n\n");
    list_t all_list = {0};
    list_init(&all_list);

    // 插入大量节点
    for (int i = 0; i < num_nodes; i++)
    {
        test_data_t data = {i, "Node"};
        list_insert_tail(&all_list, list_node_create(&data));
    }

    // 验证链表长度
    ASSERT(list_length_get(&all_list) == num_nodes, "Expected length: %d, Actual length: %d", num_nodes, list_length_get(&all_list));

    // 反复全部出表再全部入表
    for (int i = 0; i < 3; i++)
    {
        // 全部出表
        for (int j = 0; j < num_nodes; j++)
        {
            list_destroy(&all_list, list_get_head(&all_list));
        }

        // 验证链表长度
        ASSERT(list_length_get(&all_list) == 0, "Expected length: 0, Actual length: %d", list_length_get(&all_list));

        // 全部入表
        for (int j = 0; j < num_nodes; j++)
        {
            test_data_t data = {i, "Node"};
            list_insert_tail(&all_list, list_node_create(&data));
        }

        // 验证链表长度
        ASSERT(list_length_get(&all_list) == num_nodes, "Expected length: %d, Actual length: %d", num_nodes, list_length_get(&all_list));
    }

    // 销毁链表
    list_deinit(&all_list);

    dprint("Test for removing and inserting all nodes (repeated 3 times) passed!\r\n");

    dprint("All tests passed!\r\n\n\n");
}
