#!/usr/bin/env python3
import sys
import pandas as pd
import numpy as np
import matplotlib
import matplotlib.pyplot as plt
import scipy.optimize as optim
import matplotlib.ticker as plticker
from io import StringIO
from fit_functions import (geometric_shifted_func,
                           log_normal_func,
                           power_series_truncated_b1_b2_func)
from plot_parameters import plot_params

def parse_data(input_file, nskiprows=0):
    read_array = []
    with open(input_file) as file:
        read_array = file.read().splitlines()

    # count_lines = read_array.count()
    header_index = 0 + nskiprows
    data_index = 1 + nskiprows
    name = read_array[header_index].split(" ")[1].split(":")[0]
    data = pd.read_csv(StringIO(read_array[data_index]), delimiter=r"\s+", header=None).transpose()
    # return a serie (squeeze)
    return data.squeeze()

def fit_geometric_shifted_func(data):
    """ only data with degree >=3 is analyzed """
    data_filtered = data[data >= 3]
    compute_breaks = [(i - 0.5) for i in range(3, data_filtered.max() + 2)]
    # compute_breaks = [i for i in range(3, data_filtered.max() + 1)]
    counts, breaks = np.histogram(data_filtered, bins=compute_breaks, density=True)
    bin_w = np.abs(breaks[1] - breaks[0])
    centers = np.arange(breaks[0] + bin_w / 2.0, breaks[-1], bin_w)
    # area = np.sum(np.diff(breaks) * counts)
    degree_filtered_mean = data_filtered.mean()
    initial_guess = degree_filtered_mean
    popt, pcov = optim.curve_fit(geometric_shifted_func, centers, counts, initial_guess)
    fit_Z = popt[0]
    return fit_Z, (centers, counts, breaks), degree_filtered_mean, pcov

def plot_degrees(data, title=""):
    fit_Z, histo, degree_filtered_mean, pcov = fit_geometric_shifted_func(data)
    centers, counts, breaks = histo
    # Plot
    color_scatter = 'black'
    color_fit = 'C1'
    color_parameters = 'C2'
    fig, ax = plt.subplots()
    ax.set_title(title)
    ax.set_xlabel('degree')
    ax.set_ylabel('$N(p)$')
    ax.scatter(centers, counts, color=color_scatter, figure=fig)
    ax.plot(centers, geometric_shifted_func(centers, fit_Z),
            label='Fit to data:\n Z = ' + "{0:.3f}".format(fit_Z),
            color=color_fit, figure=fig)
    ax.plot(centers, geometric_shifted_func(centers, degree_filtered_mean),
            label='With Parameters:\n Z = ' + "{0:.3f}".format(degree_filtered_mean),
            color=color_parameters, figure=fig)
    # print only integer ticks.
    ax.xaxis.set_major_locator(plticker.MaxNLocator(integer=True))
    ax.legend()
    return fig, ax

def fit_log_normal(data, nbins):
    counts, breaks = np.histogram(data, bins=nbins, density=True)
    bin_w = np.abs(breaks[1] - breaks[0])
    centers = np.arange(breaks[0] + bin_w / 2.0, breaks[-1], bin_w)
    # area = np.sum(np.diff(breaks) * counts)
    log_data_mean = np.log(data).mean()
    log_data_std_dev = np.log(data).std(ddof=0)
    initial_guess = [log_data_mean, log_data_std_dev]
    func = log_normal_func
    # func = gamma_func
    popt, pcov = optim.curve_fit(func, centers, counts, initial_guess)
    fit_mu = popt[0]
    fit_std_dev = popt[1]
    # data_mean = data.mean()
    # data_std_dev = data.std(ddof=0)
    # data_mu = np.log(data_mean) # WRONG
    # data_s = np.log(data_std_dev) # WRONG
    # other_way_mu = np.log(data_mean / np.sqrt(1.0 + np.divide(data_std_dev, data_mean)**2 )) #RIGHT (less precise)
    return fit_mu, fit_std_dev, (centers, counts, breaks), log_data_mean, log_data_std_dev, pcov

def plot_distances(data, nbins, title=""):
    fit_mu, fit_std_dev, histo, log_data_mean, log_data_std_dev, pcov = fit_log_normal(data, nbins)
    centers, counts, breaks = histo
    # PLOT
    color_scatter = 'black'
    color_fit = 'C1'
    color_parameters = 'C2'
    fig, ax = plt.subplots()
    ax.set_title(title)
    ax.set_xlabel('l')
    # ax.set_yscale("log", nonposy='clip')
    ax.set_ylabel('$P(l)$')
    ax.scatter(centers, counts, color=color_scatter, figure=fig)
    func = log_normal_func
    # func = gamma_func
    ax.plot(centers, func(centers, fit_mu, fit_std_dev),
            label='Fit to data:\n $\mu$ = ' + "{0:.3f}".format(fit_mu) +
            '\n s = ' + "{0:.3f}".format(fit_std_dev),
            color=color_fit, figure=fig)
    ax.plot(centers, func(centers, log_data_mean, log_data_std_dev),
            label='With Parameters:\n $\mu$ = ' + "{0:.3f}".format(log_data_mean) +
            '\n s = ' + "{0:.3f}".format(log_data_std_dev),
            color=color_parameters, figure=fig)
    ax.legend()
    return fig, ax

def fit_power_series_truncated_b1_b2_func(data, nbins):
    counts, breaks = np.histogram(data, bins=nbins, density=True)
    bin_w = np.abs(breaks[1] - breaks[0])
    centers = np.arange(breaks[0] + bin_w / 2.0, breaks[-1], bin_w)
    func = power_series_truncated_b1_b2_func
    popt, pcov = optim.curve_fit(func, centers, counts)
    fit_b1 = popt[0]
    fit_b2 = popt[1]
    return fit_b1, fit_b2, (centers, counts, breaks), pcov

def plot_cosines(data, nbins, title=""):
    fit_b1, fit_b2, histo, pcov = fit_power_series_truncated_b1_b2_func(data, nbins)
    centers, counts, breaks = histo
    # PLOT
    color_scatter = 'black'
    color_fit = 'C1'
    fig, ax = plt.subplots()
    ax.set_title(title)
    ax.set_xlabel(r'$\beta$')
    ax.set_ylabel(r'$B(\beta)$')
    ax.scatter(centers, counts, color=color_scatter, figure=fig)
    func = power_series_truncated_b1_b2_func
    ax.plot(centers, func(centers, fit_b1, fit_b2),
            label='Fit to data:\n $b_1$ = ' + "{0:.3f}".format(fit_b1) +
            '\n $b_2$ = ' + "{0:.3f}".format(fit_b2),
            color=color_fit, figure=fig)
    # color_parameters = 'C2'
    # ax.plot(centers, func(centers, log_data_mean, log_data_std_dev),
    # label='With Parameters:\n $b_1$ = ' + "{0:.3f}".format(log_data_mean) +
    # '\n $b_2$ = ' + "{0:.3f}".format(log_data_std_dev),
    # color=color_parameters, figure=fig)
    ax.legend()
    return fig, ax


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: " + sys.argv[0] + "graph_data_file.txt [bins=30] [log_plot=False]")
        sys.exit(1)
    print("fit_to_distribution_from_data %s" % sys.argv[1])
    print("arguments: %s" % len(sys.argv))
    input_bins = 30
    log_plot = False
    if(len(sys.argv) >= 3):
        input_bins = int(sys.argv[2])
    if(len(sys.argv) == 4):
        log_plot = True
    print("input_bins %d" % input_bins)
    print("log_plot %s" % log_plot)

    input_filename = sys.argv[1]

    # ########### Plot Parameters ###############
    fig_size = (8, 6)
    plt.rcParams.update(plot_params)

    # ############ Degree ###############
    degree_row = 0
    degrees = parse_data(input_filename, degree_row)
    fig_degree, ax_degree = plot_degrees(degrees, "Degree")
    fig_degree.set_size_inches(fig_size)
    if(log_plot):
        ax_degree.set_yscale("log", nonposy='clip')
    else:
        ax_degree.set_yscale("linear")

    # ############ Ete Distance ###############
    ete_distance_row = 2
    ete_distances = parse_data(input_filename, ete_distance_row)
    ete_bins = input_bins
    fig_ete, ax_ete = plot_distances(ete_distances, ete_bins, "End To End Node Distance")
    fig_ete.set_size_inches(fig_size)
    if(log_plot):
        ax_ete.set_yscale("log", nonposy='clip')
    else:
        ax_ete.set_yscale("linear")

    # ########### Cosines ###############
    cosines_row = 6
    cosines = parse_data(input_filename, cosines_row)
    cosines_bins = input_bins
    fig_cosines, ax_cosines = plot_cosines(cosines, cosines_bins, "End To End Node Distance")
    fig_cosines.set_size_inches(fig_size)

    plt.show()
