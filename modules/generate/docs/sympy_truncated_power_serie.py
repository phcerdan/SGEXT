import argparse
description = 'Truncated (m=3) Power Series: b1*(1-x)+ b2(1-x)**3 + b3(1-x)**5'
parser = argparse.ArgumentParser(description=description)
parser.add_argument('--b1', nargs='?', default = 0.62, type=float)
parser.add_argument('--B', nargs='?', type=float, default=0.93, help='BMax is the value of the function when x=-1: Bmax = f(-1)')
args = parser.parse_args()
print(args)
print(description)
import sympy
import sympy.plotting.plot as plot
x, y, b1,b2,b3, z = sympy.symbols('x, y, b1, b2, b3, z')
z = 1 - x
# truncated series (m=3)
eq = b1*z + b2 * z**3 + b3 * z **5
# The domain of integration is [-1, 1]
cdf = sympy.integrate(eq, (x, -1, x))
normalize_eq = cdf.subs(x,1)
# We normalize CDF to 1
b3_sol = sympy.solve(normalize_eq - 1, b3)[0]
# B corresponds to eq(-1)
B_sol = eq.subs(x, -1)
# There are the two variables/free parameters left.
B_value = args.B
b1_value = args.b1
sol = sympy.solve((B_sol - B_value, b3_sol - b3, b1 - b1_value))
print("Solution:")
print(sol)
# Show eq
legend_value = True
p1 = plot( eq.subs(b1, sol[b1]).subs(b2, sol[b2]).subs(b3,sol[b3]), (x, -1, 1), legend=legend_value, show=False)
p1[0].line_color = 'b'
p1[0].label = 'PowerSeries:\nb1={}\nB={}'.format(b1_value,B_value)
p2 = plot( cdf.subs(b1, sol[b1]).subs(b2, sol[b2]).subs(b3,sol[b3]), (x, -1, 1), legend=legend_value, show=False)
p2[0].line_color = 'r'
p2[0].label = 'CDF'
p1.append(p2[0])
p1.show()


