#include "./kalman_filter.h"

// 初始化卡尔曼滤波器
void kalman_filter_init(kalman_filter_t *filter, float q, float r)
{
    // 断言检查，确保传入的指针不为空
    ASSERT(filter != NULL);
    // 断言检查，确保过程噪声协方差和测量噪声协方差大于0
    ASSERT(q > 0);
    ASSERT(r > 0);

    // 设置滤波器的过程噪声协方差和测量噪声协方差
    filter->q = q;
    filter->r = r;

    // 初始化估计状态和估计误差协方差为0
    filter->x_est = 0.0f;
    filter->p_est = 0.0f;

    // 初始化预测状态和预测误差协方差为0
    filter->x_pred = 0.0f;
    filter->p_pred = 0.0f;

    // 初始化卡尔曼增益为0
    filter->k_gain = 0.0f;
}

// 更新卡尔曼滤波器的输出值
float kalman_filter(kalman_filter_t *filter, float input)
{
    ASSERT(filter != NULL);

    // 预测步骤
    filter->x_pred = filter->x_est;
    filter->p_pred = filter->p_est + filter->q;

    // 更新步骤
    filter->k_gain = filter->p_pred / (filter->p_pred + filter->r);
    filter->x_est = filter->x_pred + filter->k_gain * (input - filter->x_pred);
    filter->p_est = (1 - filter->k_gain) * filter->p_pred;

    return filter->x_est;
}

// 自适应调整过程噪声协方差和测量噪声协方差
void kalman_filter_adapt(kalman_filter_t *filter, float measurement_error)
{
    ASSERT(filter != NULL);

    // 根据测量误差调整过程噪声协方差和测量噪声协方差
    if (fabs(measurement_error) > 1.0f)
    {
        filter->q *= 1.1f; // 增加过程噪声协方差
        filter->r *= 0.9f; // 减少测量噪声协方差
    }
    else
    {
        filter->q *= 0.9f; // 减少过程噪声协方差
        filter->r *= 1.1f; // 增加测量噪声协方差
    }

    // 确保 q 和 r 不会变成负值
    if (filter->q < 1e-6f)
        filter->q = 1e-6f;
    if (filter->r < 1e-6f)
        filter->r = 1e-6f;
}
