clc; clear; close all;

fond=35;
bordure=25;
temps_glissement=48;
Taille_fenetre=1/3;
multiplicateur_ralenti = 1/2;


M = readtable('Graphe.xlsx','VariableNamingRule','preserve');

vars = M.Properties.VariableNames;

[L,C] = size(M);
    
f=figure(1);
f.Position = [100 100 1920*Taille_fenetre 1080*Taille_fenetre];

    
    h = {animatedline('Color','r','Linewidth',2), animatedline('Color','y','LineStyle','none','Linewidth',1),animatedline('Color','g','Linewidth',2)};
    title(vars{C},'Color','w');
    xlabel(vars{1});
    ylabel(vars{C-1});
    lgnd=legend(vars(2:C-2));
    set(gca, 'Color',[fond fond fond]/255, 'XColor','w', 'YColor','w');
    set(gcf, 'Color',[bordure bordure bordure]/255);
    legend boxoff;
    set(lgnd,'TextColor','w')
    xlim([M{1,1} M{1,1}+temps_glissement]);
    
for i = 1:L
    
    for k = 2:C-2
        addpoints(h{k-1},M{i,1},M{i,k});
        hold on;
    end
    hold off;
    drawnow;
    
    set(gca, 'YLimSpec', 'Tight');
    
    if i == L        
    else
         pause(abs(M{i,1}-M{i+1,1})*multiplicateur_ralenti);       
    end
    
    if abs(M{1,1}-M{i,1}) > temps_glissement
        xlim([M{i,1}-temps_glissement M{i,1}]);
    elseif abs(M{1,1}-M{i,1}) > 0.1
        xlim([M{1,1} M{i,1}]);
    end


end