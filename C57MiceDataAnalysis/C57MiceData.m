clear         % clear all variables
clc;          % clear command window
close all;    % close all figures

%% Uploading & formating C57BL/6 mice data

% uploading CSV file
Data = readtable('CGDpheno2.csv'); 

% re-formatting data
Data([1,2,3,4,5], :) = []; % deleting first 5 rows
Data.Properties.VariableNames = {'Strain', 'Sex', 'ID', 'Age', 'TestDate', 'Parameter', 'Value'}; % renaming columns
Data.Value = str2double(Data.Value); % converting values to doubles

% extracting 'bodyweight' values
weight_indices = strcmp(Data.Parameter, 'bodyweight');
Weight = nonzeros(weight_indices.*Data.Value);

% extracting 'bodylength' values
length_indices = strcmp(Data.Parameter, 'bodylength');
Length = nonzeros(length_indices.*Data.Value);

%% Analyzing data

% removing outliers
Weight = rmoutliers(Weight, 'median'); % Outliers are defined as elements more than three scaled
Length = rmoutliers(Length, 'median'); % median absolute deviations (MAD) from the median

% summary statistics
Rows = ["Body Weight"; "Body Length"];
Mean = [mean(Weight); mean(Length)];
Q1 = [quantile(Weight, 0.25); quantile(Length, 0.25)];
Median = [median(Weight); median(Length)];
Q3 = [quantile(Weight, 0.75); quantile(Length, 0.75)];
Mode = [mode(Weight); mode(Length)];
StandardDeviation = [std(Weight); std(Length)];
Minimum = [min(Weight); min(Length)];
Maximum = [max(Weight); max(Length)];

% compiling metrics into a table
Statistics = table(Rows, Mean, Q1, Median, Q3, Mode, StandardDeviation, Minimum, Maximum)

%% Plotting data

% number of samples
NW = numel(Weight); 
NL = numel(Length);

Size = 100; % size of scatter plot markers

% BOXPLOT: WEIGHT %
figure(1); hold on; grid on; box on;
boxplot(Weight, 'Colors', [0.08 0.21 0.26]);

% superimposing weight data points to boxplot
DW = scatter(ones(NW, 1), Weight, Size, [0.06 0.55 0.55], 'filled');
alpha(DW, 0.2) % setting transparency

ylabel('Body Weight (g)')
set(gca, 'xticklabel', {'C57BL/6 Mice'}, 'fontname', 'Palatino Linotype', 'FontSize', 30)

% BOXPLOT: LENGTH %
figure(2); hold on; grid on; box on;
boxplot(Length, 'Colors', [0.08 0.21 0.26]);

% superimposing length data points to boxplot
DL = scatter(ones(NL, 1), Length, Size, [0.06 0.55 0.55], 'filled');
alpha(DL, 0.2) % setting transparency

ylabel('Body Length (cm)')
set(gca, 'xticklabel', {'C57BL/6 Mice'}, 'fontname', 'Palatino Linotype', 'FontSize', 30)

%% Reference

% Center for Genome Dynamics. (2009) Variation over time for C57BL/6J controls, multi-system survey of mouse physiology.
% https://phenome.jax.org/projects/CGDpheno2?strainid=7 .
