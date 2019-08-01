import argparse
description = ''
parser = argparse.ArgumentParser(description=description)
parser.add_argument('--normal_mean', nargs='?', default = 2.0, type=float, help='mean of the variable x (not of log(x))')
parser.add_argument('--normal_std_deviation', nargs='?', type=float, default=0.5, help='std deviation of the normal variable')
args = parser.parse_args()
print(args)
print(description)
m_value = args.normal_mean
s_value = args.normal_std_deviation
import sympy
import sympy.plotting.plot as plot
from sympy.stats import LogNormal, density
# m and s are the non-log mean and std deviation, respectively
# mu and sigma are the log mean and std deviation, respectively
x,z, m, s, mu, sigma = sympy.symbols('x, z, m, s, mu, sigma')

sigma = sympy.sqrt(sympy.log(1 + s**2/m**2))
mu = sympy.log(m) - sigma**2/2.0
# mu = sympy.Symbol('mu', real=True)
# sigma = sympy.Symbol('sigma', positive=True)

func = LogNormal('x', mu, sigma)
D = density(func)(x)
CDF = density(func).cdf(x)
mu_value = mu.subs(m, m_value).subs(s, s_value)
sigma_value = sigma.subs(m, m_value).subs(s, s_value)

print("mu: {}\nsigma {}".format(mu_value, sigma_value))
# Show eq
legend_value = True
x_max =  3*m_value
p1 = plot( D.subs(mu, mu_value).subs(sigma,sigma_value), (x, 0.001, x_max), legend=legend_value, show=False)
p1[0].line_color = 'b'
p1[0].label = 'LogNormal:\nmu={}\nsigma={}'.format(mu_value,sigma_value)
p2 = plot( CDF.subs(mu, mu_value).subs(sigma,sigma_value), (x, 0.001, x_max), legend=legend_value, show=False)
p2[0].line_color = 'r'
p2[0].label = 'CDF'
p1.append(p2[0])
p1.show()


