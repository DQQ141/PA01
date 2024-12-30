
#include "./fit.h"

// 预测函数实现
float fit_predict(fit_cfg_t *cfg, fit_params_t params, int x)
{
    return params.a * cfg->func(params.b * x);
}

// 拟合函数实现
fit_params_t fit_training(fit_cfg_t *cfg, fit_data_t *data, uint32_t n)
{
    // 初始化参数
    float sum_x = 0, sum_y = 0, sum_xy = 0, sum_x2 = 0, sum_log_y = 0, sum_x_log_y = 0;

    // 计算各项和
    for (int i = 0; i < n; ++i)
    {
        float log_y = cfg->defunc(data[i].y);
        sum_x += data[i].x;
        sum_x2 += data[i].x * data[i].x;
        sum_log_y += log_y;
        sum_x_log_y += data[i].x * log_y;
    }

    // 计算参数 a 和 b
    float denominator = n * sum_x2 - sum_x * sum_x;
    float a = (sum_log_y * sum_x2 - sum_x * sum_x_log_y) / denominator;
    float b = (n * sum_x_log_y - sum_x * sum_log_y) / denominator;

    fit_params_t params;
    params.a = cfg->func(a);
    params.b = b;

    if (cfg->enable_r2)
    {
        float mean_y = sum_log_y / n;
        float ss_res = 0; // 残差平方和（Sum of Squares Residuals），表示模型未能解释的方差
        float ss_tot = 0; // 总平方和（Total Sum of Squares），表示观测数据的方差

        // 计算 ss_res，ss_tot
        for (int i = 0; i < n; ++i)
        {
            float predicted_y = fit_predict(cfg, params, data[i].x);
            ss_res += powf(data[i].y - predicted_y, 2);
            ss_tot += powf(data[i].y - mean_y, 2);
        }

        // 计算R^2
        params.r2 = 1 - (ss_res / ss_tot);
    }
    else
    {
        params.r2 = 0;
    }

    return params;
}
