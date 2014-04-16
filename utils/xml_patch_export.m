function xml_patch_export(nrb,filename,dim_phys)
% 
% date: 2014/16/04
% author: nicola cavallini
%
% XML exporting for a single pathc geometry
% generated with nurbs tookbox.
%
% xml_patch_export(nrb,filename,dim_phys)
%
% nrb = structure defining the nurbs.
% filename = output file without extention.
% dim_phys = dimension of the curren domain.
%
nrb_node = com.mathworks.xml.XMLUtils.createDocument... 
    ('XMLFile')
%
nrb_root = nrb_node.getDocumentElement;
%
%
% Patch
%
dim_ref = length(nrb.knots);
%
patch_element = nrb_node.createElement('Patch');
%
patch_element.setAttribute('DimPhysicalDomain',num2str(dim_phys));
patch_element.setAttribute('DimReferenceDomain',num2str(dim_ref));
patch_element.setAttribute('ID','0');
%
nrb_root.appendChild(patch_element);
%
%
order = nrb.order;
%
for cnt =1:dim_ref
   %
   [break_points,m,n] = unique(nrb.knots{cnt});
   %
   mult = zeros(size(break_points));
   mult(1) = m(1);
   mult(2:end) = diff(m);
   %
   knot_element = nrb_node.createElement('KnotVector');
   knot_element.setAttribute('Direction',num2str(cnt-1));
   knot_element.setAttribute('NumBreakPoints',num2str(...
       length(break_points)...
   ));
   %
   knot_element.setAttribute('Degree',num2str(...
     order(cnt)-1 ));
   %
   patch_element.appendChild(knot_element);
   %
   break_element = nrb_node.createElement('BreakPoints');
   break_element.setAttribute('Num',num2str(...
       length(break_points)...
   ));
   %
   for bk = 1:length(break_points)
    break_element.appendChild(nrb_node.createTextNode(sprintf(' %15e',...
    break_points(bk)...
       )) );
   end
   %
   knot_element.appendChild(break_element);
   %
   mult_element = nrb_node.createElement('Multiplicities');
   mult_element.setAttribute('Num',num2str(...
       length(break_points)...
   ));
   %
   for bk = 1:length(break_points)
    mult_element.appendChild(nrb_node.createTextNode(sprintf(' %i',...
    mult(bk)...
       )) );
   end
   %
   knot_element.appendChild(mult_element);
   %
end
%
coeff_num = nrb.number;
%
coeff_element = nrb_node.createElement('ControlPoints');
coeff_element.setAttribute('Num',num2str(prod(coeff_num)) );
patch_element.appendChild(coeff_element);
%
dir_element = nrb_node.createElement('NumDir');
for nd = 1:dim_ref
    dir_element.appendChild(nrb_node.createTextNode(sprintf(' %i',...
        coeff_num(nd)...
        )) );
end
coeff_element.appendChild(dir_element);
%
w = nrb.coefs(4,:,:);
w = w(:);
%
for dp = 1:dim_phys
    x = nrb.coefs(dp,:,:);
    x = x(:)./w;
    coord_element = nrb_node.createElement('Coordinates');
    coord_element.setAttribute('Dir',num2str(dp-1));
    coord_element.setAttribute('Num',num2str(prod(coeff_num)) );
    for cp =1:prod(coeff_num)
        coord_element.appendChild(nrb_node.createTextNode(sprintf(' %15e',...
        x(cp)...
       )) );
    end
    coeff_element.appendChild(coord_element);
end
%
w_element = nrb_node.createElement('Weights');
%w_element.setAttribute('Dir',num2str(dp-1));
w_element.setAttribute('Num',num2str(prod(coeff_num)) );
for cp =1:prod(coeff_num)
    w_element.appendChild(nrb_node.createTextNode(sprintf(' %15e',...
        w(cp)...
        )) );
end
coeff_element.appendChild(w_element);

%
%
xmlFileName = [filename,'.xml'];
xmlwrite(xmlFileName,nrb_node);
type(xmlFileName);
%
return