import numpy as np
from math import gamma

def power_func(x, A, B):
    return A * x**B

def geometric_shifted_func(p, Z):
    """
    precondition: p >= 3. Z is the average node degree
    p exists in [3, inf]
    """
    # if(p < 3):
    #     raise ValueError("geometric_shifted_func needs input to be greater than 3 but used with " + p)
    q = 1.0 / (Z - 2)
    return q * (1.0 - q)**(p - 3)

# def mixture_delta_and_geometric_shifted_func(p, Z, prob_one):
#     """
#     Z is the average node degree
#     prob_one is the probability (delta) of degree one in the pool
#     p exists in [1, inf]
#     p cannot take degree = 2
#     """
#     if(np.random.rand() <= prob_one):
#         return prob_one
#     else:
#         q = 1.0 / (Z - 2)
#         return (1 - prob_one) * (q * (1 - q)**(p - 3))

def log_normal_func(l, ln_mu, ln_std_dev):
    """
    log normal function. ln_mu is the mean of ln(l)
    l exists in [0, inf]
    """
    return (1.0 / (l * ln_std_dev * np.sqrt(2 * np.pi))) * np.exp(- (ln_mu - np.log(l))**2 / (2 * ln_std_dev**2))

def gamma_func(x, alpha, beta):
    """ gamma function  """
    return x**(alpha - 1) * np.exp(-x / beta) / (gamma(alpha) * beta**alpha)

def power_series_truncated_func(x, b1, b2):
    """
    Truncated to 3 func(x) = sum(b_k * (1-x)**(2k-1)), where k = {1,2,3}
    x is a cosine director in the interval (-1.0, 1.0]
    """
    # the distribution is normalized to 1, so there are only 2 independent parameters
    # b3 = -(3/32.0) * (-1 + 2 * b1 + 4 * b2);
    # b3 = -0.09375  * (-1 + 2 * b1 + 4 * b2);
    # b3 =  0.09375  * ( 1 - 2 * b1 - 4 * b2);
    return b1 * (1 - x) + b2 * (1 - x)**3 + 0.09375 * (1 - 2 * b1 - 4 * b2) * (1 - x)**5
