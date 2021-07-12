%
% This program is free software: you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation, either version 3 of the License, or
% (at your option) any later version.
%
% This program is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
% GNU General Public License for more details.
%
% You should have received a copy of the GNU General Public License
% along with this program.  If not, see <http://www.gnu.org/licenses/>.
%
% Author: Maxim Penner <maxim.penner@ikt.uni-hannover.de>
%

clear all;
%close all;
clc;

fn = 'PER_0_HARQ.txt';
%fn = 'PER_1_HARQ.txt';

T = readtable(fn);

A = table2array(T);

mcs_array = unique(A(:,1));

figure(1)
clf()

%legend('AutoUpdate','off')

% for legend entries HARQ
str='HARQ 0';
semilogy(0,1000,'k-','DisplayName',str);
hold on
str='HARQ 1';
semilogy(0,1000,'k--','DisplayName',str);

set(gca,'ColorOrderIndex',1)

% plot PER
for mcs=mcs_array'
    
    rows = find(A(:,1) == mcs);
    A_relevant = A(rows,:);
    
    snr_vec = A_relevant(:,2);
    PER_vec = A_relevant(:,6);
    TBS_size = A_relevant(1,7);
    
    str = ['MCS ',num2str(mcs),' TBS=',num2str(TBS_size)];
    semilogy(snr_vec, PER_vec, '-o','DisplayName',str, 'MarkerSize', 5);
end

font_size_ticks = 12;
font_size_title = 12;
font_size_axis_labels = 12;
font_size_legend = 10;

xlim([-15 20]);
ylim([1e-6 1e1]);

set(gca, 'TickLabelInterpreter', 'latex','FontSize', font_size_ticks);

xlabel('SNR (dB)', 'Interpreter','latex', 'FontSize', font_size_axis_labels)
ylabel('Packet Error Rate (PER)','Interpreter','latex', 'FontSize', font_size_axis_labels)

legend_location = 'SouthWest';
legend('Location',legend_location, 'Interpreter','latex', 'FontSize', font_size_legend);

grid on

% size pos
op = get(gcf,'OuterPosition');
set(gcf,'units','points','OuterPosition',[100,100,516,430])

% save
addpath('export_fig_lib')
export_fig PER_HARQ.png -m4 -transparent
