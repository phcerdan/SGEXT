#!/usr/bin/env python3
import sys
import pandas as pd
import numpy as np
import matplotlib
import matplotlib.pyplot as plt
import scipy.optimize as optimize
import scipy.interpolate as interpolate
import matplotlib.ticker as ticker
from io import StringIO
from fit_functions import (geometric_shifted_func,
                           log_normal_func,
                           gamma_func,
                           power_series_truncated_func)
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

def r_squared_from_curve_fit(xdata, ydata, func, popt):
    """
    Effron's pseudo r-squared.
    Read more: https://stats.idre.ucla.edu/other/mult-pkg/faq/general/faq-what-are-pseudo-r-squareds/
    From: https://stackoverflow.com/questions/19189362/getting-the-r-squared-value-using-curve-fit
    """
    # residuals sums of squares.
    residuals = ydata - func(xdata, *popt)
    ss_res = np.sum(residuals**2)
    # Total sum of squares
    ss_tot = np.sum((ydata - np.mean(ydata))**2)
    # return r-squared
    return 1 - (ss_res / ss_tot)


def fit_geometric_shifted_func(data):
    """ only data with degree >=3 is analyzed """
    data_filtered = data[data >= 3]
    compute_breaks = [(i - 0.5) for i in range(3, data_filtered.max() + 2)]
    counts, breaks = np.histogram(data_filtered, bins=compute_breaks, density=True)
    bin_w = np.abs(breaks[1] - breaks[0])
    centers = np.arange(breaks[0] + bin_w / 2.0, breaks[-1], bin_w)
    # area = np.sum(np.diff(breaks) * counts)
    degree_filtered_mean = data_filtered.mean()
    initial_guess = degree_filtered_mean
    popt, pcov = optimize.curve_fit(geometric_shifted_func, centers, counts, initial_guess)
    # fit_Z = popt[0]
    return popt, (centers, counts, breaks), pcov, (degree_filtered_mean,)

def plot_branch_degrees(data, title=""):
    """ weight allows to scale the pdf for using it in a mixture """
    popt, histo, pcov, pfitted = fit_geometric_shifted_func(data)
    fit_Z = popt[0]
    degree_filtered_mean = pfitted[0]
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
    r_squared_fit = r_squared_from_curve_fit(centers, counts, geometric_shifted_func, popt)
    ax.plot(centers, geometric_shifted_func(centers, *popt),
            label='Fit to data:\n Z = ' + "{0:.3f}".format(fit_Z) +
            '\n$R^2$ = ' + "{0:.5f}".format(r_squared_fit),
            color=color_fit, figure=fig)
    r_squared_parameters = r_squared_from_curve_fit(centers, counts, geometric_shifted_func, pfitted)
    ax.plot(centers, geometric_shifted_func(centers, *pfitted),
            label='With Parameters:\n Z = ' + "{0:.3f}".format(degree_filtered_mean) +
            '\n$R^2$ = ' + "{0:.5f}".format(r_squared_parameters),
            color=color_parameters, figure=fig)
    # print only integer ticks.
    ax.xaxis.set_major_locator(ticker.MaxNLocator(integer=True))
    ax.legend()
    return fig, ax

def fit_gamma(data, nbins):
    counts, breaks = np.histogram(data, bins=nbins, density=True)
    bin_w = np.abs(breaks[1] - breaks[0])
    centers = np.arange(breaks[0] + bin_w / 2.0, breaks[-1], bin_w)
    func = gamma_func
    popt, pcov = optimize.curve_fit(func, centers, counts)
    return popt, (centers, counts, breaks), pcov

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
    popt, pcov = optimize.curve_fit(func, centers, counts, initial_guess)
    # data_mean = data.mean()
    # data_std_dev = data.std(ddof=0)
    # data_mu = np.log(data_mean) # WRONG
    # data_s = np.log(data_std_dev) # WRONG
    # other_way_mu = np.log(data_mean / np.sqrt(1.0 + np.divide(data_std_dev, data_mean)**2 )) #RIGHT (less precise)
    return popt, (centers, counts, breaks), pcov, (log_data_mean, log_data_std_dev)

def plot_distances(data, nbins, title=""):
    func = log_normal_func
    fit_func = fit_log_normal
    popt, histo, pcov, pfitted = fit_func(data, nbins)
    # func = gamma_func
    # fit_func = fit_gamma
    # popt, histo, pcov = fit_func(data, nbins)
    centers, counts, breaks = histo
    # PLOT
    color_scatter = 'black'
    color_fit = 'C1'
    fig, ax = plt.subplots()
    ax.set_title(title)
    ax.set_xlabel('l')
    ax.set_ylabel('$P(l)$')
    ax.scatter(centers, counts, color=color_scatter, figure=fig)
    r_squared_fit = r_squared_from_curve_fit(centers, counts, func, popt)
    # ax.plot(centers, func(centers, *popt),
    centers_smooth = np.linspace(centers.min(), centers.max(), 200)
    ax.plot(centers_smooth, interpolate.spline(centers, func(centers, *popt), centers_smooth),
            label='Fit to data:\n $\mu_l$ = ' + "{0:.3E}".format(np.exp(popt[0])) +
            '\n $s_l$ = ' + "{0:.3f}".format(popt[1]) +
            '\n$R^2$ = ' + "{0:.5f}".format(r_squared_fit),
            color=color_fit, figure=fig)

    color_parameters = 'C2'
    r_squared_parameters = r_squared_from_curve_fit(centers, counts, func, pfitted)
    # ax.plot(centers, func(centers, *pfitted),
    ax.plot(centers_smooth, interpolate.spline(centers, func(centers, *pfitted), centers_smooth),
            label='With Parameters:\n $\mu_l$ = ' + "{0:.3E}".format(np.exp(pfitted[0])) +
            '\n $s_l$ = ' + "{0:.3f}".format(pfitted[1]) +
            '\n$R^2$ = ' + "{0:.5f}".format(r_squared_parameters),
            color=color_parameters, figure=fig)
    ax.set_xlim(0.0, breaks[-1] + (breaks[1] - breaks[0]))
    ax.xaxis.set_major_formatter(ticker.StrMethodFormatter("{x:.2E}"))

    ax.legend()
    return fig, ax

def fit_power_series_truncated_func(data, nbins):
    counts, breaks = np.histogram(data, bins=nbins, density=True)
    bin_w = np.abs(breaks[1] - breaks[0])
    centers = np.arange(breaks[0] + bin_w / 2.0, breaks[-1], bin_w)
    func = power_series_truncated_func
    popt, pcov = optimize.curve_fit(func, centers, counts)
    # fit_b1 = popt[0]
    # fit_b2 = popt[1]
    return popt, (centers, counts, breaks), pcov

def plot_cosines(data, nbins, title=""):
    popt, histo, pcov = fit_power_series_truncated_func(data, nbins)
    fit_b1 = popt[0]
    fit_b2 = popt[1]
    centers, counts, breaks = histo
    # PLOT
    color_scatter = 'black'
    color_fit = 'C1'
    fig, ax = plt.subplots()
    ax.set_title(title)
    ax.set_xlabel(r'$\beta$')
    ax.set_ylabel(r'$B(\beta)$')
    ax.scatter(centers, counts, color=color_scatter, figure=fig)
    func = power_series_truncated_func
    r_squared_fit = r_squared_from_curve_fit(centers, counts, func, popt)
    ax.plot(centers, func(centers, *popt),
            label='Fit to data:\n $b_1$ = ' + "{0:.3E}".format(fit_b1) +
            '\n $b_2$ = ' + "{0:.3E}".format(fit_b2) +
            '\n$R^2$ = ' + "{0:.5f}".format(r_squared_fit),
            color=color_fit, figure=fig)
    ax.legend()
    return fig, ax


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: " + sys.argv[0] +
              "graph_data_file.txt [ete_bins=30] [cosines_bins=21] " +
              "[contour_bins = 50] [log_plot=False]")
        sys.exit(1)
    print("fit_to_distribution_from_data %s" % sys.argv[1])
    print("arguments: %s" % len(sys.argv))
    input_ete_bins = 30
    input_cosines_bins = 21
    input_contour_bins = 50
    log_plot = False
    if(len(sys.argv) >= 3):
        input_ete_bins = int(sys.argv[2])
    if(len(sys.argv) >= 4):
        input_cosines_bins = int(sys.argv[3])
    if(len(sys.argv) >= 5):
        input_contour_bins = int(sys.argv[4])
    if(len(sys.argv) == 6):
        log_plot = True
    print("input_ete_bins %d" % input_ete_bins)
    print("input_cosines_bins %d" % input_cosines_bins)
    print("input_contour_bins %d" % input_contour_bins)
    print("log_plot %s" % log_plot)

    input_filename = sys.argv[1]

    # ########### Plot Parameters ###############
    fig_size = (8, 6)
    plt.rcParams.update(plot_params)

    # ############ Degree ###############
    degree_row = 0
    degrees = parse_data(input_filename, degree_row)

    # Compute the ratio of degree 1 versus branch points (degree=>3).
    compute_degree_breaks = [(i - 0.5) for i in range(1, degrees.max() + 2)]
    degree_counts, degree_breaks = np.histogram(degrees, bins=compute_degree_breaks)
    count_one = degree_counts[0]
    prob_degree_one = count_one / (count_one + degree_counts[2:].sum())
    degree_bin_w = np.abs(degree_breaks[1] - degree_breaks[0])
    degree_centers = np.arange(degree_breaks[0] + degree_bin_w / 2.0, degree_breaks[-1], degree_bin_w)
    degree_counts_one = np.zeros(degree_counts.size)
    degree_counts_one[0] = prob_degree_one
    print("Ratio of end nodes %s" % prob_degree_one)
    # weight = 1 - prob_degree_one
    # Change weight to show the
    # degree_counts_one[0] = prob_degree_one / (1 - prob_degree_one)

    # Plot the branch_degrees (weighted)
    fig_degree, ax_degree = plot_branch_degrees(degrees, "Degree")
    # Plot the degree one (weighted)
    ax_degree.scatter(degree_centers[0], degree_counts_one[0], color='C0',
                      label='Ratio of end nodes: ' + "{0:.3f}".format(prob_degree_one), figure=fig_degree)
    ax_degree.legend()
    fig_degree.set_size_inches(fig_size)
    ax_degree.set_yscale("log", nonposy='clip')
    # if(log_plot):
    #     ax_degree.set_yscale("log", nonposy='clip')
    # else:
    #     ax_degree.set_yscale("linear")

    # ############ Ete Distance ###############
    ete_distance_row = 2
    ete_distances = parse_data(input_filename, ete_distance_row)
    fig_ete, ax_ete = plot_distances(ete_distances, input_ete_bins, "End To End Node Distance")
    fig_ete.set_size_inches(fig_size)
    if(log_plot):
        ax_ete.set_yscale("log", nonposy='clip')
    else:
        ax_ete.set_yscale("linear")

    # ########### Cosines ###############
    cosines_row = 6
    cosines = parse_data(input_filename, cosines_row)
    fig_cosines, ax_cosines = plot_cosines(cosines, input_cosines_bins, "Director Cosines")
    fig_cosines.set_size_inches(fig_size)

    # ############ Contour Lengths ###############
    contour_distance_row = 8
    contour_distances = parse_data(input_filename, contour_distance_row)
    fig_contour, ax_contour = plot_distances(contour_distances, input_contour_bins, "Contour Lengths")
    fig_contour.set_size_inches(fig_size)
    if(log_plot):
        ax_contour.set_yscale("log", nonposy='clip')
    else:
        ax_contour.set_yscale("linear")

    # ############ Show ###############
    plt.show()

