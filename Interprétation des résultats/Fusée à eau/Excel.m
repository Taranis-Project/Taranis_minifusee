clear all
clc
close all

a = 0

table = 'tpsgzhyjxc';
u = zeros(length(table),1);

%permet de lire le txt
fid = fopen('data_excel.txt');
tline = fgetl(fid);

%reinitialise l'excel
filename = 'data.xlsx';
delete(filename);

%%%%%%% CORPS DU PROGRAMME
while ischar(tline)
    %disp(tline) %permet d'afficher la ligne actuelle
    
    zer = tline;
    expression = '[a-z][-]?[\d]+';
    matchStr = regexp(zer,expression,'match');
    
    m = max(size(matchStr));
    
    
    for i = 1:m 
        
        
        for j = 1:length(table)
            
            if matchStr{i}(1) == table(j)
                
                u(j) = str2num(matchStr{i}(2:end));
                
            end
            
        end
       % ancien code 
%         if matchStr{i}(1) == table(i)
%             
%             u(i) = str2num(matchStr{i}(2:end));
%             
%         end
%         j = i;
%         while j < length(table)+1
%             
%             if matchStr{i}(1) == table(j)
%                 
%                 u(j) = str2num(matchStr{i}(2:end));
%                 j = length(table);
%             end
%             j = j + 1;
%             
%         end

    end
    
    if u(2) ~= 0
        u(2) = u(2) + 100000;
    end
    
    a = a + 1;
    v = ['A' num2str(a) ':J' num2str(a)]
    writematrix(u.',filename,'Sheet',2,'Range',v);
    
    tline = fgetl(fid); %IMPORTANT sinon boucle infini
end
fclose(fid);

%%%%%%%%%%%%%

disp('EXECUTION TERMINER')
