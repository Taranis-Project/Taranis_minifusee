clc; clear; close all;

multiplicateur_ralenti = 2;


M = readtable('Graphe.xlsx','VariableNamingRule','preserve');

vars = M.Properties.VariableNames;

[L,C] = size(M);
    
figure(1);
    
    h = {animatedline('Color','r'), animatedline('Color','b'),animatedline('Color','g')};
    title(vars{C});
    xlabel(vars{1});
    ylabel(vars{C-1});
    legend(vars(2:C-2));
    xlim([M{1,1} M{1,1}+30]);
    
for i = 1:L
    
    for k = 2:C-2
        addpoints(h{k-1},M{i,1},M{i,k});
        hold on;
    end
    hold off;
    drawnow;
    
    if i == L        
    else
         pause(abs(M{i,1}-M{i+1,1})*multiplicateur_ralenti);       
    end
    
    if abs(M{1,1}-M{i,1}) > 30
    xlim([M{i,1}-30 M{i,1}]);
    end

end