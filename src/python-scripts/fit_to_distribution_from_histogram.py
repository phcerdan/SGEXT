import pandas as pd
import numpy as np
import matplotlib
import matplotlib.pyplot as plt
import scipy.optimize as optim
import os
import matplotlib.ticker as plticker
from fit_functions import geometric_shifted_func, log_normal_func

from io import StringIO
def parse_histo_data(input_file, nskiprows=0):
    read_array = []
    with open(input_file) as file:
        read_array = file.read().splitlines()

    # count_lines = read_array.count()

    header_index = 0 + nskiprows
    center_index = 1 + nskiprows
    count_index = 2 + nskiprows
    name = read_array[header_index].split(" ")[1].split(":")[0]
    centers = pd.read_csv(StringIO(read_array[center_index]), delimiter = r"\s+", header=None)
    counts = pd.read_csv(StringIO(read_array[count_index]), delimiter = r"\s+", header=None)

    output = centers.transpose()
    output['counts'] = counts.transpose()
    output.columns = ['centers','counts']
    return output


# small for debug
histo_path = "/home/phc/repository_local/network-graph/results/Carrageenan/Dgraphs/p2/histograms/CarrKbin1_1792_INV_4x4x1_tile_1_tv20.0_SegmentRG_l138_u5000_p0.05_Hole_M3_R1_N10000_SKEL_dmax_1isthmus_p2_reduced_c_m_iPA_iShort1_bD0_bA25_bC25_wL1.000000.histo"
# histo_path = "/home/phc/repository_local/network-graph/results/Actin/Dgraphs/p2/histograms/Actin19_11jun13_2_Series2_WSimoncelli_L4_B4_AnisDenoise_t0.0625_N20_c2.0_SegmentRG_l0_u5000_p0.2_CROPPED_Hole_M3_R1_N10000_2x2x2_tile_6_SKEL_dmax_1isthmus_p2_reduced_c_m_iShort5_bD0_bA25_bC25_wL1.000000.histo"
fig_size = (8,6)

fig_degree, ax_degree = plt.subplots(figsize=fig_size)

degree_raw = parse_histo_data(histo_path, 0)
degree = degree_raw[3:]
# normalize
degree.loc[:, 'counts'] /= degree.sum()['counts']
plt.plot(degree['centers'], degree['counts'], 'o', figure=fig_degree, label='Data', color='blue')

# fit not needed, we actually know value of mean_degree
# degree_popt, degree_pcov = optim.curve_fit(geometric_shifted_func, degree['centers'], degree['counts'])
# degree_perr = np.sqrt(np.diag(degree_pcov))
# popt = degree_popt
# perr = degree_perr
degree_mean = (degree['centers'] * degree['counts']).sum()
# b_value_error = [ popt[1], perr[1] ]
# plt.plot(xdata, power_func(xdata, *popt), '--', figure=fig_degree, label='low-q: B=%3.3f (%1.3f)' % tuple(b_value_error))
plt.plot(degree['centers'], geometric_shifted_func(degree['centers'], degree_mean), figure=fig_degree, label='Power-Law Fit', color='black')
ax_degree.set_yscale("log")
fig_degree.show()

fig_ete_distance, ax_ete_distance = plt.subplots(figsize=fig_size)
ete_distance_raw = parse_histo_data(histo_path, 4)
ete_distance = ete_distance_raw
# normalize
ete_distance.loc[:, 'counts'] /= ete_distance.sum()['counts']
plt.plot(ete_distance['centers'], ete_distance['counts'], 'o', figure=fig_ete_distance, label='Data', color='blue')

ete_distance_popt, ete_distance_pcov = optim.curve_fit(log_normal_func, ete_distance['centers'], ete_distance['counts'])
ete_distance_perr = np.sqrt(np.diag(ete_distance_pcov))
popt = ete_distance_popt
perr = ete_distance_perr
# It is normalized, so we don't have to divide by the sum of values (it is 1.0)
ete_distance_countsxcenters =  ete_distance['centers'] * ete_distance['counts']
ete_distance_l_mean = ete_distance_countsxcenters.sum()
# ete_distance_l_std_dev_manual = np.sqrt(( (ete_distance['centers'] - ete_distance_l_mean )**2  * (ete_distance['counts'] )).sum() )
ete_distance_l_std_dev = (ete_distance['centers'] * ete_distance['counts']).std(ddof=0)
ete_distance_mu = np.log(ete_distance_l_mean / np.sqrt(1 + np.divide(ete_distance_l_std_dev,ete_distance_l_mean)**2 ))
ete_distance_std_dev = np.sqrt( np.log( 1 + np.divide(ete_distance_l_std_dev,ete_distance_l_mean)**2 ) )
ete_distance_ln_mean_directly = (np.log(ete_distance['centers']) * ete_distance['counts']).sum()
ete_distance_ln_std_dev_directly = (np.log(ete_distance['centers']) * ete_distance['counts']).std(ddof=0)
# ete_distance_ln_mean = np.log( (ete_distance['centers'] * ete_distance['counts']).sum())
# ete_distance_ln_std_dev = np.log( (ete_distance['centers'] * ete_distance['counts']).std(ddof=0))
# ete_distance_ln_std_dev = np.log( ete_distance['centers'].std() )
# b_value_error = [ popt[1], perr[1] ]
# plt.plot(xdata, power_func(xdata, *popt), '--', figure=fig_ete_distance, label='low-q: B=%3.3f (%1.3f)' % tuple(b_value_error))
plt.plot(ete_distance['centers'], log_normal_func(ete_distance['centers'], *popt), figure=fig_ete_distance, label='Power-Law Fit', color='black')
# plt.plot(ete_distance['centers'], log_normal_func(ete_distance['centers'], ete_distance_ln_mean, ete_distance_ln_std_dev), figure=fig_ete_distance, label='Power-Law Fit', color='black')
ax_ete_distance.set_yscale("log")
fig_ete_distance.show()
