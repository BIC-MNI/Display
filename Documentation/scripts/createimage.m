figure;
img = imread('rotated.png');
M = size(img,1);
N = size(img,2);
image(img);
ax = gca;
xticks = {};
yticks = {};
C = 30;
NI = round(N/C);
MI = round(M/C);
for i=0:NI,
    xticks{i+1} = sprintf('%d', i*2);
end

for i=0:MI,
    yticks{i+1} = sprintf('%d',  i*2);
end
hold on;
for k=0:C/2:M,
    k
    x = [0 (N-1)];
    y = [k k];
    plot(x, y, 'Color', 'k', 'LineStyle', ':');
end
for k=0:C/2:N,
    x = [k k];
    y = [0 (M-1)];
    plot(x,y,'Color', 'k', 'LineStyle', ':');
end
hold off;

set(ax, 'XTick', [0:C:N]);
set(ax, 'YTick', [0:C:M]);
set(ax, 'XTickLabel', xticks);
set(ax, 'YTickLabel', yticks);

xlabel('Voxel j axis');
ylabel('Voxel k axis');

print -dpng coordinates.png
