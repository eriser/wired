#!/bin/sh

url_svn="https://wired.svn.sourceforge.net/svnroot/wired/"
export_root="./"
output_file=${export_root}"/ChangeLog"
temp_file="./generateChangeLog.tmp"
url_svn_tags=${url_svn}"/tags"
url_svn_trunk=${url_svn}"/trunk"

#Echoes the last changed revision for a given svn url
getLastChangedRevForUrl() {
  url=${1}

  last_rev=$( svn info ${url} | grep "Last Changed Rev" | grep -o -e "[0-9]\+$" )

  echo ${last_rev}
}


#On prend l'avant dernièr (n-1 ligne car on est censé avoir déjà taggé.
echo "Getting last release tag..."
last_tag=$( svn ls ${url_svn_tags} | tail -1 ) 

#On récupère le numéro de la derniere revision de ce tag
echo "Getting last revision for tag [${last_tag}]..."
last_revision=$( getLastChangedRevForUrl ${url_svn_tags}/${last_tag} )

#On récupère le numéro de la revision courante
echo "Getting current revision..."
current_revision=$( getLastChangedRevForUrl ${url_svn}/"trunk" );

#On fait le bordel pour écrire le changelog au début du fichier
echo "Adding logs between revision [${last_revision}] and HEAD"
#On s'assure qu'il y a quelque chose
touch ${output_file}
#On sauvegarde l'ancien fichier de log
cp ${output_file} ${temp_file}
#On réinitialise le fichier avec le nouvel en tete
echo "${last_tag} (revisions : ${last_revision} - ${current_revision})" > ${output_file}
echo "------------------------------------------------------------------------" >> ${output_file}
echo "" >> ${output_file}
#On ajoute le dernier changelog
#Le dernier grep est un peu une feinte mais bon...
svn log --xml -rHEAD:${last_revision} ${url_svn_trunk} | xsltproc ./transform.xsl - | grep -v -e "<?xml" >> ${output_file}
#On remet l'ancien changelog derrière
cat ${temp_file} >> ${output_file}
rm ${temp_file}
