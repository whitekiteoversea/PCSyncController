data = readmatrix('CAS1_SDRAMFileSave_export_2024_07_23_14_37_49.csv');

% 提取第二列和第三列数据
x = data(:, 2);
y = data(:, 3);

xmin = 1000;
xmax = 20000;

ymin = 8697903;
ymax = 9203551;

% 过滤无效数据
validIndices = (x >= xmin) & (x <= xmax);
x_filtered = x(validIndices);
validIndices = (y >= ymin) & (y <= ymax);
y_filtered = y(validIndices);

% 检查过滤后的数据
disp(x_filtered);
disp(y_filtered);
% 生成折线图
figure;
plot(x_filtered, y_filtered, '-o');
xlabel('Second Column Data');
ylabel('Third Column Data');
title('Plot of Second Column vs Third Column');
grid on;
