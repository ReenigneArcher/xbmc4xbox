/*
** FAAD2 - Freeware Advanced Audio (AAC) Decoder including SBR decoding
** Copyright (C) 2003 M. Bakker, Ahead Software AG, http://www.nero.com
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software 
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
**
** Any non-GPL usage of this software or parts of this software is strictly
** forbidden.
**
** Commercial non-GPL licensing of this software is possible.
** For more info contact Ahead Software through Mpeg4AAClicense@nero.com.
**
** $Id$
**/

#ifndef __SBR_NOISE_H__
#define __SBR_NOISE_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
#pragma warning(disable:4305)
#pragma warning(disable:4244)
#endif


/* Table 1.A.13 Noise table V */
complex_t V[] = {
    { COEF_CONST(-0.99948155879974), COEF_CONST(-0.59483414888382) },
    { COEF_CONST(0.97113454341888), COEF_CONST(-0.67528516054153) },
    { COEF_CONST(0.14130051434040), COEF_CONST(-0.95090985298157) },
    { COEF_CONST(-0.47005495429039), COEF_CONST(-0.37340548634529) },
    { COEF_CONST(0.80705064535141), COEF_CONST(0.29653668403625) },
    { COEF_CONST(-0.38981479406357), COEF_CONST(0.89572608470917) },
    { COEF_CONST(-0.01053049881011), COEF_CONST(-0.66959059238434) },
    { COEF_CONST(-0.91266369819641), COEF_CONST(-0.11522938311100) },
    { COEF_CONST(0.54840421676636), COEF_CONST(0.75221365690231) },
    { COEF_CONST(0.40009254217148), COEF_CONST(-0.98929399251938) },
    { COEF_CONST(-0.99867975711823), COEF_CONST(-0.88147068023682) },
    { COEF_CONST(-0.95531076192856), COEF_CONST(0.90908759832382) },
    { COEF_CONST(-0.45725932717323), COEF_CONST(-0.56716322898865) },
    { COEF_CONST(-0.72929674386978), COEF_CONST(-0.98008275032043) },
    { COEF_CONST(0.75622802972794), COEF_CONST(0.20950329303741) },
    { COEF_CONST(0.07069442421198), COEF_CONST(-0.78247898817062) },
    { COEF_CONST(0.74496251344681), COEF_CONST(-0.91169005632401) },
    { COEF_CONST(-0.96440184116364), COEF_CONST(-0.94739919900894) },
    { COEF_CONST(0.30424630641937), COEF_CONST(-0.49438267946243) },
    { COEF_CONST(0.66565030813217), COEF_CONST(0.64652937650681) },
    { COEF_CONST(0.91697007417679), COEF_CONST(0.17514097690582) },
    { COEF_CONST(-0.70774918794632), COEF_CONST(0.52548652887344) },
    { COEF_CONST(-0.70051413774490), COEF_CONST(-0.45340028405190) },
    { COEF_CONST(-0.99496513605118), COEF_CONST(-0.90071910619736) },
    { COEF_CONST(0.98164492845535), COEF_CONST(-0.77463155984879) },
    { COEF_CONST(-0.54671579599380), COEF_CONST(-0.02570928446949) },
    { COEF_CONST(-0.01689629070461), COEF_CONST(0.00287506449968) },
    { COEF_CONST(-0.86110347509384), COEF_CONST(0.42548584938049) },
    { COEF_CONST(-0.98892980813980), COEF_CONST(-0.87881129980087) },
    { COEF_CONST(0.51756626367569), COEF_CONST(0.66926783323288) },
    { COEF_CONST(-0.99635028839111), COEF_CONST(-0.58107727766037) },
    { COEF_CONST(-0.99969369173050), COEF_CONST(0.98369991779327) },
    { COEF_CONST(0.55266261100769), COEF_CONST(0.59449058771133) },
    { COEF_CONST(0.34581178426743), COEF_CONST(0.94879418611526) },
    { COEF_CONST(0.62664210796356), COEF_CONST(-0.74402970075607) },
    { COEF_CONST(-0.77149701118469), COEF_CONST(-0.33883658051491) },
    { COEF_CONST(-0.91592246294022), COEF_CONST(0.03687901422381) },
    { COEF_CONST(-0.76285493373871), COEF_CONST(-0.91371870040894) },
    { COEF_CONST(0.79788339138031), COEF_CONST(-0.93180972337723) },
    { COEF_CONST(0.54473078250885), COEF_CONST(-0.11919206380844) },
    { COEF_CONST(-0.85639280080795), COEF_CONST(0.42429855465889) },
    { COEF_CONST(-0.92882400751114), COEF_CONST(0.27871808409691) },
    { COEF_CONST(-0.11708371341228), COEF_CONST(-0.99800843000412) },
    { COEF_CONST(0.21356749534607), COEF_CONST(-0.90716296434402) },
    { COEF_CONST(-0.76191693544388), COEF_CONST(0.99768120050430) },
    { COEF_CONST(0.98111045360565), COEF_CONST(-0.95854461193085) },
    { COEF_CONST(-0.85913270711899), COEF_CONST(0.95766568183899) },
    { COEF_CONST(-0.93307244777679), COEF_CONST(0.49431759119034) },
    { COEF_CONST(0.30485755205154), COEF_CONST(-0.70540034770966) },
    { COEF_CONST(0.85289651155472), COEF_CONST(0.46766132116318) },
    { COEF_CONST(0.91328084468842), COEF_CONST(-0.99839597940445) },
    { COEF_CONST(-0.05890199914575), COEF_CONST(0.70741826295853) },
    { COEF_CONST(0.28398686647415), COEF_CONST(0.34633556008339) },
    { COEF_CONST(0.95258164405823), COEF_CONST(-0.54893416166306) },
    { COEF_CONST(-0.78566324710846), COEF_CONST(-0.75568538904190) },
    { COEF_CONST(-0.95789498090744), COEF_CONST(-0.20423194766045) },
    { COEF_CONST(0.82411158084869), COEF_CONST(0.96654617786407) },
    { COEF_CONST(-0.65185445547104), COEF_CONST(-0.88734990358353) },
    { COEF_CONST(-0.93643605709076), COEF_CONST(0.99870789051056) },
    { COEF_CONST(0.91427159309387), COEF_CONST(-0.98290503025055) },
    { COEF_CONST(-0.70395684242249), COEF_CONST(0.58796799182892) },
    { COEF_CONST(0.00563771976158), COEF_CONST(0.61768198013306) },
    { COEF_CONST(0.89065051078796), COEF_CONST(0.52783352136612) },
    { COEF_CONST(-0.68683707714081), COEF_CONST(0.80806946754456) },
    { COEF_CONST(0.72165340185165), COEF_CONST(-0.69259858131409) },
    { COEF_CONST(-0.62928247451782), COEF_CONST(0.13627037405968) },
    { COEF_CONST(0.29938435554504), COEF_CONST(-0.46051329374313) },
    { COEF_CONST(-0.91781955957413), COEF_CONST(-0.74012714624405) },
    { COEF_CONST(0.99298715591431), COEF_CONST(0.40816611051559) },
    { COEF_CONST(0.82368296384811), COEF_CONST(-0.74036049842834) },
    { COEF_CONST(-0.98512834310532), COEF_CONST(-0.99972331523895) },
    { COEF_CONST(-0.95915371179581), COEF_CONST(-0.99237799644470) },
    { COEF_CONST(-0.21411126852036), COEF_CONST(-0.93424820899963) },
    { COEF_CONST(-0.68821477890015), COEF_CONST(-0.26892307400703) },
    { COEF_CONST(0.91851997375488), COEF_CONST(0.09358228743076) },
    { COEF_CONST(-0.96062767505646), COEF_CONST(0.36099094152451) },
    { COEF_CONST(0.51646184921265), COEF_CONST(-0.71373331546783) },
    { COEF_CONST(0.61130720376968), COEF_CONST(0.46950140595436) },
    { COEF_CONST(0.47336128354073), COEF_CONST(-0.27333179116249) },
    { COEF_CONST(0.90998309850693), COEF_CONST(0.96715664863586) },
    { COEF_CONST(0.44844800233841), COEF_CONST(0.99211573600769) },
    { COEF_CONST(0.66614890098572), COEF_CONST(0.96590173244476) },
    { COEF_CONST(0.74922239780426), COEF_CONST(-0.89879858493805) },
    { COEF_CONST(-0.99571585655212), COEF_CONST(0.52785521745682) },
    { COEF_CONST(0.97401082515717), COEF_CONST(-0.16855870187283) },
    { COEF_CONST(0.72683745622635), COEF_CONST(-0.48060774803162) },
    { COEF_CONST(0.95432192087173), COEF_CONST(0.68849605321884) },
    { COEF_CONST(-0.72962206602097), COEF_CONST(-0.76608443260193) },
    { COEF_CONST(-0.85359477996826), COEF_CONST(0.88738125562668) },
    { COEF_CONST(-0.81412428617477), COEF_CONST(-0.97480767965317) },
    { COEF_CONST(-0.87930774688721), COEF_CONST(0.74748307466507) },
    { COEF_CONST(-0.71573328971863), COEF_CONST(-0.98570609092712) },
    { COEF_CONST(0.83524298667908), COEF_CONST(0.83702534437180) },
    { COEF_CONST(-0.48086065053940), COEF_CONST(-0.98848503828049) },
    { COEF_CONST(0.97139126062393), COEF_CONST(0.80093622207642) },
    { COEF_CONST(0.51992827653885), COEF_CONST(0.80247628688812) },
    { COEF_CONST(-0.00848591234535), COEF_CONST(-0.76670128107071) },
    { COEF_CONST(-0.70294374227524), COEF_CONST(0.55359911918640) },
    { COEF_CONST(-0.95894426107407), COEF_CONST(-0.43265503644943) },
    { COEF_CONST(0.97079253196716), COEF_CONST(0.09325857460499) },
    { COEF_CONST(-0.92404294013977), COEF_CONST(0.85507702827454) },
    { COEF_CONST(-0.69506472349167), COEF_CONST(0.98633414506912) },
    { COEF_CONST(0.26559203863144), COEF_CONST(0.73314309120178) },
    { COEF_CONST(0.28038442134857), COEF_CONST(0.14537914097309) },
    { COEF_CONST(-0.74138122797012), COEF_CONST(0.99310338497162) },
    { COEF_CONST(-0.01752796024084), COEF_CONST(-0.82616633176804) },
    { COEF_CONST(-0.55126774311066), COEF_CONST(-0.98898541927338) },
    { COEF_CONST(0.97960901260376), COEF_CONST(-0.94021445512772) },
    { COEF_CONST(-0.99196308851242), COEF_CONST(0.67019015550613) },
    { COEF_CONST(-0.67684930562973), COEF_CONST(0.12631492316723) },
    { COEF_CONST(0.09140039235353), COEF_CONST(-0.20537731051445) },
    { COEF_CONST(-0.71658962965012), COEF_CONST(-0.97788202762604) },
    { COEF_CONST(0.81014639139175), COEF_CONST(0.53722649812698) },
    { COEF_CONST(0.40616992115974), COEF_CONST(-0.26469007134438) },
    { COEF_CONST(-0.67680186033249), COEF_CONST(0.94502049684525) },
    { COEF_CONST(0.86849772930145), COEF_CONST(-0.18333598971367) },
    { COEF_CONST(-0.99500381946564), COEF_CONST(-0.02634122036397) },
    { COEF_CONST(0.84329187870026), COEF_CONST(0.10406957566738) },
    { COEF_CONST(-0.09215968847275), COEF_CONST(0.69540011882782) },
    { COEF_CONST(0.99956172704697), COEF_CONST(-0.12358541786671) },
    { COEF_CONST(-0.79732781648636), COEF_CONST(-0.91582524776459) },
    { COEF_CONST(0.96349972486496), COEF_CONST(0.96640455722809) },
    { COEF_CONST(-0.79942780733109), COEF_CONST(0.64323902130127) },
    { COEF_CONST(-0.11566039919853), COEF_CONST(0.28587844967842) },
    { COEF_CONST(-0.39922955632210), COEF_CONST(0.94129604101181) },
    { COEF_CONST(0.99089199304581), COEF_CONST(-0.92062628269196) },
    { COEF_CONST(0.28631284832954), COEF_CONST(-0.91035044193268) },
    { COEF_CONST(-0.83302724361420), COEF_CONST(-0.67330408096313) },
    { COEF_CONST(0.95404446125031), COEF_CONST(0.49162766337395) },
    { COEF_CONST(-0.06449863314629), COEF_CONST(0.03250560909510) },
    { COEF_CONST(-0.99575054645538), COEF_CONST(0.42389783263206) },
    { COEF_CONST(-0.65501141548157), COEF_CONST(0.82546114921570) },
    { COEF_CONST(-0.81254440546036), COEF_CONST(-0.51627236604691) },
    { COEF_CONST(-0.99646371603012), COEF_CONST(0.84490531682968) },
    { COEF_CONST(0.00287840608507), COEF_CONST(0.64768260717392) },
    { COEF_CONST(0.70176988840103), COEF_CONST(-0.20453028380871) },
    { COEF_CONST(0.96361881494522), COEF_CONST(0.40706968307495) },
    { COEF_CONST(-0.68883758783340), COEF_CONST(0.91338956356049) },
    { COEF_CONST(-0.34875586628914), COEF_CONST(0.71472293138504) },
    { COEF_CONST(0.91980081796646), COEF_CONST(0.66507452726364) },
    { COEF_CONST(-0.99009048938751), COEF_CONST(0.85868018865585) },
    { COEF_CONST(0.68865793943405), COEF_CONST(0.55660319328308) },
    { COEF_CONST(-0.99484401941299), COEF_CONST(-0.20052559673786) },
    { COEF_CONST(0.94214510917664), COEF_CONST(-0.99696427583694) },
    { COEF_CONST(-0.67414629459381), COEF_CONST(0.49548220634460) },
    { COEF_CONST(-0.47339352965355), COEF_CONST(-0.85904330015182) },
    { COEF_CONST(0.14323651790619), COEF_CONST(-0.94145596027374) },
    { COEF_CONST(-0.29268294572830), COEF_CONST(0.05759225040674) },
    { COEF_CONST(0.43793860077858), COEF_CONST(-0.78904968500137) },
    { COEF_CONST(-0.36345127224922), COEF_CONST(0.64874434471130) },
    { COEF_CONST(-0.08750604838133), COEF_CONST(0.97686946392059) },
    { COEF_CONST(-0.96495270729065), COEF_CONST(-0.53960305452347) },
    { COEF_CONST(0.55526942014694), COEF_CONST(0.78891521692276) },
    { COEF_CONST(0.73538213968277), COEF_CONST(0.96452075242996) },
    { COEF_CONST(-0.30889773368835), COEF_CONST(-0.80664390325546) },
    { COEF_CONST(0.03574995696545), COEF_CONST(-0.97325617074966) },
    { COEF_CONST(0.98720687627792), COEF_CONST(0.48409134149551) },
    { COEF_CONST(-0.81689298152924), COEF_CONST(-0.90827703475952) },
    { COEF_CONST(0.67866861820221), COEF_CONST(0.81284505128860) },
    { COEF_CONST(-0.15808570384979), COEF_CONST(0.85279554128647) },
    { COEF_CONST(0.80723392963409), COEF_CONST(-0.24717418849468) },
    { COEF_CONST(0.47788757085800), COEF_CONST(-0.46333149075508) },
    { COEF_CONST(0.96367555856705), COEF_CONST(0.38486748933792) },
    { COEF_CONST(-0.99143874645233), COEF_CONST(-0.24945276975632) },
    { COEF_CONST(0.83081877231598), COEF_CONST(-0.94780850410461) },
    { COEF_CONST(-0.58753192424774), COEF_CONST(0.01290772389621) },
    { COEF_CONST(0.95538109540939), COEF_CONST(-0.85557049512863) },
    { COEF_CONST(-0.96490919589996), COEF_CONST(-0.64020973443985) },
    { COEF_CONST(-0.97327101230621), COEF_CONST(0.12378127872944) },
    { COEF_CONST(0.91400367021561), COEF_CONST(0.57972472906113) },
    { COEF_CONST(-0.99925839900970), COEF_CONST(0.71084845066071) },
    { COEF_CONST(-0.86875903606415), COEF_CONST(-0.20291699469090) },
    { COEF_CONST(-0.26240035891533), COEF_CONST(-0.68264555931091) },
    { COEF_CONST(-0.24664412438869), COEF_CONST(-0.87642270326614) },
    { COEF_CONST(0.02416275814176), COEF_CONST(0.27192914485931) },
    { COEF_CONST(0.82068622112274), COEF_CONST(-0.85087788105011) },
    { COEF_CONST(0.88547372817993), COEF_CONST(-0.89636802673340) },
    { COEF_CONST(-0.18173077702522), COEF_CONST(-0.26152145862579) },
    { COEF_CONST(0.09355476498604), COEF_CONST(0.54845124483109) },
    { COEF_CONST(-0.54668414592743), COEF_CONST(0.95980775356293) },
    { COEF_CONST(0.37050989270210), COEF_CONST(-0.59910142421722) },
    { COEF_CONST(-0.70373594760895), COEF_CONST(0.91227668523788) },
    { COEF_CONST(-0.34600785374641), COEF_CONST(-0.99441426992416) },
    { COEF_CONST(-0.68774479627609), COEF_CONST(-0.30238837003708) },
    { COEF_CONST(-0.26843291521072), COEF_CONST(0.83115667104721) },
    { COEF_CONST(0.49072334170341), COEF_CONST(-0.45359709858894) },
    { COEF_CONST(0.38975992798805), COEF_CONST(0.95515358448029) },
    { COEF_CONST(-0.97757124900818), COEF_CONST(0.05305894464254) },
    { COEF_CONST(-0.17325553297997), COEF_CONST(-0.92770671844482) },
    { COEF_CONST(0.99948036670685), COEF_CONST(0.58285546302795) },
    { COEF_CONST(-0.64946246147156), COEF_CONST(0.68645507097244) },
    { COEF_CONST(-0.12016920745373), COEF_CONST(-0.57147324085236) },
    { COEF_CONST(-0.58947455883026), COEF_CONST(-0.34847131371498) },
    { COEF_CONST(-0.41815140843391), COEF_CONST(0.16276422142982) },
    { COEF_CONST(0.99885648488998), COEF_CONST(0.11136095225811) },
    { COEF_CONST(-0.56649613380432), COEF_CONST(-0.90494865179062) },
    { COEF_CONST(0.94138020277023), COEF_CONST(0.35281917452812) },
    { COEF_CONST(-0.75725078582764), COEF_CONST(0.53650552034378) },
    { COEF_CONST(0.20541973412037), COEF_CONST(-0.94435143470764) },
    { COEF_CONST(0.99980372190475), COEF_CONST(0.79835915565491) },
    { COEF_CONST(0.29078277945518), COEF_CONST(0.35393777489662) },
    { COEF_CONST(-0.62858772277832), COEF_CONST(0.38765692710876) },
    { COEF_CONST(0.43440905213356), COEF_CONST(-0.98546332120895) },
    { COEF_CONST(-0.98298585414886), COEF_CONST(0.21021524071693) },
    { COEF_CONST(0.19513028860092), COEF_CONST(-0.94239830970764) },
    { COEF_CONST(-0.95476663112640), COEF_CONST(0.98364555835724) },
    { COEF_CONST(0.93379634618759), COEF_CONST(-0.70881992578506) },
    { COEF_CONST(-0.85235410928726), COEF_CONST(-0.08342348039150) },
    { COEF_CONST(-0.86425095796585), COEF_CONST(-0.45795026421547) },
    { COEF_CONST(0.38879778981209), COEF_CONST(0.97274428606033) },
    { COEF_CONST(0.92045122385025), COEF_CONST(-0.62433654069901) },
    { COEF_CONST(0.89162534475327), COEF_CONST(0.54950958490372) },
    { COEF_CONST(-0.36834338307381), COEF_CONST(0.96458297967911) },
    { COEF_CONST(0.93891763687134), COEF_CONST(-0.89968353509903) },
    { COEF_CONST(0.99267655611038), COEF_CONST(-0.03757034242153) },
    { COEF_CONST(-0.94063472747803), COEF_CONST(0.41332337260246) },
    { COEF_CONST(0.99740225076675), COEF_CONST(-0.16830494999886) },
    { COEF_CONST(-0.35899412631989), COEF_CONST(-0.46633225679398) },
    { COEF_CONST(0.05237237364054), COEF_CONST(-0.25640362501144) },
    { COEF_CONST(0.36703583598137), COEF_CONST(-0.38653266429901) },
    { COEF_CONST(0.91653180122375), COEF_CONST(-0.30587628483772) },
    { COEF_CONST(0.69000804424286), COEF_CONST(0.90952169895172) },
    { COEF_CONST(-0.38658750057220), COEF_CONST(0.99501574039459) },
    { COEF_CONST(-0.29250815510750), COEF_CONST(0.37444993853569) },
    { COEF_CONST(-0.60182201862335), COEF_CONST(0.86779648065567) },
    { COEF_CONST(-0.97418588399887), COEF_CONST(0.96468526124954) },
    { COEF_CONST(0.88461571931839), COEF_CONST(0.57508403062820) },
    { COEF_CONST(0.05198933184147), COEF_CONST(0.21269661188126) },
    { COEF_CONST(-0.53499621152878), COEF_CONST(0.97241556644440) },
    { COEF_CONST(-0.49429559707642), COEF_CONST(0.98183864355087) },
    { COEF_CONST(-0.98935145139694), COEF_CONST(-0.40249159932137) },
    { COEF_CONST(-0.98081380128860), COEF_CONST(-0.72856897115707) },
    { COEF_CONST(-0.27338150143623), COEF_CONST(0.99950921535492) },
    { COEF_CONST(0.06310802698135), COEF_CONST(-0.54539585113525) },
    { COEF_CONST(-0.20461677014828), COEF_CONST(-0.14209978282452) },
    { COEF_CONST(0.66223841905594), COEF_CONST(0.72528582811356) },
    { COEF_CONST(-0.84764343500137), COEF_CONST(0.02372316829860) },
    { COEF_CONST(-0.89039862155914), COEF_CONST(0.88866579532623) },
    { COEF_CONST(0.95903307199478), COEF_CONST(0.76744925975800) },
    { COEF_CONST(0.73504126071930), COEF_CONST(-0.03747203201056) },
    { COEF_CONST(-0.31744435429573), COEF_CONST(-0.36834111809731) },
    { COEF_CONST(-0.34110826253891), COEF_CONST(0.40211221575737) },
    { COEF_CONST(0.47803884744644), COEF_CONST(-0.39423218369484) },
    { COEF_CONST(0.98299193382263), COEF_CONST(0.01989791356027) },
    { COEF_CONST(-0.30963072180748), COEF_CONST(-0.18076720833778) },
    { COEF_CONST(0.99992591142654), COEF_CONST(-0.26281872391701) },
    { COEF_CONST(-0.93149733543396), COEF_CONST(-0.98313164710999) },
    { COEF_CONST(0.99923473596573), COEF_CONST(-0.80142992734909) },
    { COEF_CONST(-0.26024168729782), COEF_CONST(-0.75999760627747) },
    { COEF_CONST(-0.35712513327599), COEF_CONST(0.19298963248730) },
    { COEF_CONST(-0.99899083375931), COEF_CONST(0.74645155668259) },
    { COEF_CONST(0.86557173728943), COEF_CONST(0.55593866109848) },
    { COEF_CONST(0.33408042788506), COEF_CONST(0.86185956001282) },
    { COEF_CONST(0.99010735750198), COEF_CONST(0.04602397605777) },
    { COEF_CONST(-0.66694271564484), COEF_CONST(-0.91643613576889) },
    { COEF_CONST(0.64016789197922), COEF_CONST(0.15649530291557) },
    { COEF_CONST(0.99570536613464), COEF_CONST(0.45844584703445) },
    { COEF_CONST(-0.63431465625763), COEF_CONST(0.21079117059708) },
    { COEF_CONST(-0.07706847041845), COEF_CONST(-0.89581435918808) },
    { COEF_CONST(0.98590087890625), COEF_CONST(0.88241720199585) },
    { COEF_CONST(0.80099332332611), COEF_CONST(-0.36851897835732) },
    { COEF_CONST(0.78368133306503), COEF_CONST(0.45506998896599) },
    { COEF_CONST(0.08707806468010), COEF_CONST(0.80938994884491) },
    { COEF_CONST(-0.86811882257462), COEF_CONST(0.39347308874130) },
    { COEF_CONST(-0.39466530084610), COEF_CONST(-0.66809433698654) },
    { COEF_CONST(0.97875326871872), COEF_CONST(-0.72467839717865) },
    { COEF_CONST(-0.95038563013077), COEF_CONST(0.89563220739365) },
    { COEF_CONST(0.17005239427090), COEF_CONST(0.54683053493500) },
    { COEF_CONST(-0.76910793781281), COEF_CONST(-0.96226614713669) },
    { COEF_CONST(0.99743282794952), COEF_CONST(0.42697158455849) },
    { COEF_CONST(0.95437383651733), COEF_CONST(0.97002321481705) },
    { COEF_CONST(0.99578905105591), COEF_CONST(-0.54106825590134) },
    { COEF_CONST(0.28058260679245), COEF_CONST(-0.85361421108246) },
    { COEF_CONST(0.85256522893906), COEF_CONST(-0.64567607641220) },
    { COEF_CONST(-0.50608539581299), COEF_CONST(-0.65846014022827) },
    { COEF_CONST(-0.97210735082626), COEF_CONST(-0.23095212876797) },
    { COEF_CONST(0.95424050092697), COEF_CONST(-0.99240148067474) },
    { COEF_CONST(-0.96926569938660), COEF_CONST(0.73775655031204) },
    { COEF_CONST(0.30872163176537), COEF_CONST(0.41514959931374) },
    { COEF_CONST(-0.24523839354515), COEF_CONST(0.63206630945206) },
    { COEF_CONST(-0.33813264966011), COEF_CONST(-0.38661777973175) },
    { COEF_CONST(-0.05826828256249), COEF_CONST(-0.06940773874521) },
    { COEF_CONST(-0.22898460924625), COEF_CONST(0.97054851055145) },
    { COEF_CONST(-0.18509915471077), COEF_CONST(0.47565764188766) },
    { COEF_CONST(-0.10488238185644), COEF_CONST(-0.87769949436188) },
    { COEF_CONST(-0.71886587142944), COEF_CONST(0.78030979633331) },
    { COEF_CONST(0.99793875217438), COEF_CONST(0.90041309595108) },
    { COEF_CONST(0.57563304901123), COEF_CONST(-0.91034334897995) },
    { COEF_CONST(0.28909647464752), COEF_CONST(0.96307784318924) },
    { COEF_CONST(0.42188999056816), COEF_CONST(0.48148649930954) },
    { COEF_CONST(0.93335050344467), COEF_CONST(-0.43537023663521) },
    { COEF_CONST(-0.97087377309799), COEF_CONST(0.86636447906494) },
    { COEF_CONST(0.36722871661186), COEF_CONST(0.65291655063629) },
    { COEF_CONST(-0.81093025207520), COEF_CONST(0.08778370171785) },
    { COEF_CONST(-0.26240602135658), COEF_CONST(-0.92774093151093) },
    { COEF_CONST(0.83996498584747), COEF_CONST(0.55839848518372) },
    { COEF_CONST(-0.99909615516663), COEF_CONST(-0.96024608612061) },
    { COEF_CONST(0.74649465084076), COEF_CONST(0.12144893407822) },
    { COEF_CONST(-0.74774593114853), COEF_CONST(-0.26898062229156) },
    { COEF_CONST(0.95781666040421), COEF_CONST(-0.79047924280167) },
    { COEF_CONST(0.95472306013107), COEF_CONST(-0.08588775992393) },
    { COEF_CONST(0.48708331584930), COEF_CONST(0.99999040365219) },
    { COEF_CONST(0.46332037448883), COEF_CONST(0.10964126139879) },
    { COEF_CONST(-0.76497006416321), COEF_CONST(0.89210927486420) },
    { COEF_CONST(0.57397389411926), COEF_CONST(0.35289704799652) },
    { COEF_CONST(0.75374317169189), COEF_CONST(0.96705216169357) },
    { COEF_CONST(-0.59174400568008), COEF_CONST(-0.89405369758606) },
    { COEF_CONST(0.75087904930115), COEF_CONST(-0.29612672328949) },
    { COEF_CONST(-0.98607856035233), COEF_CONST(0.25034910440445) },
    { COEF_CONST(-0.40761056542397), COEF_CONST(-0.90045571327209) },
    { COEF_CONST(0.66929268836975), COEF_CONST(0.98629492521286) },
    { COEF_CONST(-0.97463697195053), COEF_CONST(-0.00190223299433) },
    { COEF_CONST(0.90145510435104), COEF_CONST(0.99781388044357) },
    { COEF_CONST(-0.87259286642075), COEF_CONST(0.99233585596085) },
    { COEF_CONST(-0.91529458761215), COEF_CONST(-0.15698707103729) },
    { COEF_CONST(-0.03305738791823), COEF_CONST(-0.37205263972282) },
    { COEF_CONST(0.07223051041365), COEF_CONST(-0.88805001974106) },
    { COEF_CONST(0.99498009681702), COEF_CONST(0.97094357013702) },
    { COEF_CONST(-0.74904936552048), COEF_CONST(0.99985486268997) },
    { COEF_CONST(0.04585228487849), COEF_CONST(0.99812334775925) },
    { COEF_CONST(-0.89054954051971), COEF_CONST(-0.31791913509369) },
    { COEF_CONST(-0.83782142400742), COEF_CONST(0.97637635469437) },
    { COEF_CONST(0.33454805612564), COEF_CONST(-0.86231517791748) },
    { COEF_CONST(-0.99707579612732), COEF_CONST(0.93237990140915) },
    { COEF_CONST(-0.22827528417110), COEF_CONST(0.18874759972095) },
    { COEF_CONST(0.67248046398163), COEF_CONST(-0.03646211326122) },
    { COEF_CONST(-0.05146538093686), COEF_CONST(-0.92599701881409) },
    { COEF_CONST(0.99947297573090), COEF_CONST(0.93625229597092) },
    { COEF_CONST(0.66951125860214), COEF_CONST(0.98905825614929) },
    { COEF_CONST(-0.99602955579758), COEF_CONST(-0.44654715061188) },
    { COEF_CONST(0.82104903459549), COEF_CONST(0.99540740251541) },
    { COEF_CONST(0.99186509847641), COEF_CONST(0.72022998332977) },
    { COEF_CONST(-0.65284591913223), COEF_CONST(0.52186721563339) },
    { COEF_CONST(0.93885445594788), COEF_CONST(-0.74895310401917) },
    { COEF_CONST(0.96735250949860), COEF_CONST(0.90891814231873) },
    { COEF_CONST(-0.22225968539715), COEF_CONST(0.57124030590057) },
    { COEF_CONST(-0.44132784008980), COEF_CONST(-0.92688840627670) },
    { COEF_CONST(-0.85694974660873), COEF_CONST(0.88844531774521) },
    { COEF_CONST(0.91783040761948), COEF_CONST(-0.46356892585754) },
    { COEF_CONST(0.72556972503662), COEF_CONST(-0.99899554252625) },
    { COEF_CONST(-0.99711579084396), COEF_CONST(0.58211559057236) },
    { COEF_CONST(0.77638977766037), COEF_CONST(0.94321835041046) },
    { COEF_CONST(0.07717324048281), COEF_CONST(0.58638399839401) },
    { COEF_CONST(-0.56049829721451), COEF_CONST(0.82522302865982) },
    { COEF_CONST(0.98398894071579), COEF_CONST(0.39467439055443) },
    { COEF_CONST(0.47546947002411), COEF_CONST(0.68613046407700) },
    { COEF_CONST(0.65675091743469), COEF_CONST(0.18331636488438) },
    { COEF_CONST(0.03273375332355), COEF_CONST(-0.74933111667633) },
    { COEF_CONST(-0.38684144616127), COEF_CONST(0.51337349414825) },
    { COEF_CONST(-0.97346270084381), COEF_CONST(-0.96549361944199) },
    { COEF_CONST(-0.53282153606415), COEF_CONST(-0.91423267126083) },
    { COEF_CONST(0.99817311763763), COEF_CONST(0.61133575439453) },
    { COEF_CONST(-0.50254499912262), COEF_CONST(-0.88829338550568) },
    { COEF_CONST(0.01995873264968), COEF_CONST(0.85223513841629) },
    { COEF_CONST(0.99930381774902), COEF_CONST(0.94578897953033) },
    { COEF_CONST(0.82907766103745), COEF_CONST(-0.06323442608118) },
    { COEF_CONST(-0.58660709857941), COEF_CONST(0.96840775012970) },
    { COEF_CONST(-0.17573736608028), COEF_CONST(-0.48166921734810) },
    { COEF_CONST(0.83434289693832), COEF_CONST(-0.13023450970650) },
    { COEF_CONST(0.05946491286159), COEF_CONST(0.20511047542095) },
    { COEF_CONST(0.81505483388901), COEF_CONST(-0.94685947895050) },
    { COEF_CONST(-0.44976380467415), COEF_CONST(0.40894573926926) },
    { COEF_CONST(-0.89746475219727), COEF_CONST(0.99846577644348) },
    { COEF_CONST(0.39677256345749), COEF_CONST(-0.74854665994644) },
    { COEF_CONST(-0.07588948309422), COEF_CONST(0.74096214771271) },
    { COEF_CONST(0.76343196630478), COEF_CONST(0.41746628284454) },
    { COEF_CONST(-0.74490106105804), COEF_CONST(0.94725912809372) },
    { COEF_CONST(0.64880120754242), COEF_CONST(0.41336661577225) },
    { COEF_CONST(0.62319535017014), COEF_CONST(-0.93098312616348) },
    { COEF_CONST(0.42215818166733), COEF_CONST(-0.07712787389755) },
    { COEF_CONST(0.02704554051161), COEF_CONST(-0.05417517945170) },
    { COEF_CONST(0.80001771450043), COEF_CONST(0.91542196273804) },
    { COEF_CONST(-0.79351830482483), COEF_CONST(-0.36208897829056) },
    { COEF_CONST(0.63872361183167), COEF_CONST(0.08128252625465) },
    { COEF_CONST(0.52890521287918), COEF_CONST(0.60048872232437) },
    { COEF_CONST(0.74238550662994), COEF_CONST(0.04491915181279) },
    { COEF_CONST(0.99096131324768), COEF_CONST(-0.19451183080673) },
    { COEF_CONST(-0.80412328243256), COEF_CONST(-0.88513815402985) },
    { COEF_CONST(-0.64612615108490), COEF_CONST(0.72198677062988) },
    { COEF_CONST(0.11657770723104), COEF_CONST(-0.83662831783295) },
    { COEF_CONST(-0.95053184032440), COEF_CONST(-0.96939903497696) },
    { COEF_CONST(-0.62228870391846), COEF_CONST(0.82767260074615) },
    { COEF_CONST(0.03004475869238), COEF_CONST(-0.99738895893097) },
    { COEF_CONST(-0.97987216711044), COEF_CONST(0.36526128649712) },
    { COEF_CONST(-0.99986982345581), COEF_CONST(-0.36021611094475) },
    { COEF_CONST(0.89110648632050), COEF_CONST(-0.97894251346588) },
    { COEF_CONST(0.10407960414886), COEF_CONST(0.77357792854309) },
    { COEF_CONST(0.95964735746384), COEF_CONST(-0.35435819625854) },
    { COEF_CONST(0.50843232870102), COEF_CONST(0.96107691526413) },
    { COEF_CONST(0.17006334662437), COEF_CONST(-0.76854026317596) },
    { COEF_CONST(0.25872674584389), COEF_CONST(0.99893301725388) },
    { COEF_CONST(-0.01115998718888), COEF_CONST(0.98496019840240) },
    { COEF_CONST(-0.79598701000214), COEF_CONST(0.97138410806656) },
    { COEF_CONST(-0.99264711141586), COEF_CONST(-0.99542820453644) },
    { COEF_CONST(-0.99829661846161), COEF_CONST(0.01877138763666) },
    { COEF_CONST(-0.70801013708115), COEF_CONST(0.33680686354637) },
    { COEF_CONST(-0.70467054843903), COEF_CONST(0.93272775411606) },
    { COEF_CONST(0.99846023321152), COEF_CONST(-0.98725748062134) },
    { COEF_CONST(-0.63364970684052), COEF_CONST(-0.16473594307899) },
    { COEF_CONST(-0.16258217394352), COEF_CONST(-0.95939123630524) },
    { COEF_CONST(-0.43645593523979), COEF_CONST(-0.94805032014847) },
    { COEF_CONST(-0.99848473072052), COEF_CONST(0.96245169639587) },
    { COEF_CONST(-0.16796459257603), COEF_CONST(-0.98987513780594) },
    { COEF_CONST(-0.87979227304459), COEF_CONST(-0.71725726127625) },
    { COEF_CONST(0.44183099269867), COEF_CONST(-0.93568974733353) },
    { COEF_CONST(0.93310177326202), COEF_CONST(-0.99913311004639) },
    { COEF_CONST(-0.93941932916641), COEF_CONST(-0.56409376859665) },
    { COEF_CONST(-0.88590002059937), COEF_CONST(0.47624599933624) },
    { COEF_CONST(0.99971461296082), COEF_CONST(-0.83889955282211) },
    { COEF_CONST(-0.75376385450363), COEF_CONST(0.00814643409103) },
    { COEF_CONST(0.93887686729431), COEF_CONST(-0.11284527927637) },
    { COEF_CONST(0.85126435756683), COEF_CONST(0.52349251508713) },
    { COEF_CONST(0.39701420068741), COEF_CONST(0.81779634952545) },
    { COEF_CONST(-0.37024465203285), COEF_CONST(-0.87071657180786) },
    { COEF_CONST(-0.36024826765060), COEF_CONST(0.34655734896660) },
    { COEF_CONST(-0.93388813734055), COEF_CONST(-0.84476542472839) },
    { COEF_CONST(-0.65298801660538), COEF_CONST(-0.18439576029778) },
    { COEF_CONST(0.11960318684578), COEF_CONST(0.99899345636368) },
    { COEF_CONST(0.94292563199997), COEF_CONST(0.83163905143738) },
    { COEF_CONST(0.75081145763397), COEF_CONST(-0.35533222556114) },
    { COEF_CONST(0.56721979379654), COEF_CONST(-0.24076835811138) },
    { COEF_CONST(0.46857765316963), COEF_CONST(-0.30140233039856) },
    { COEF_CONST(0.97312313318253), COEF_CONST(-0.99548190832138) },
    { COEF_CONST(-0.38299977779388), COEF_CONST(0.98516911268234) },
    { COEF_CONST(0.41025799512863), COEF_CONST(0.02116736955941) },
    { COEF_CONST(0.09638062119484), COEF_CONST(0.04411984235048) },
    { COEF_CONST(-0.85283249616623), COEF_CONST(0.91475564241409) },
    { COEF_CONST(0.88866806030273), COEF_CONST(-0.99735265970230) },
    { COEF_CONST(-0.48202428221703), COEF_CONST(-0.96805608272552) },
    { COEF_CONST(0.27572581171989), COEF_CONST(0.58634752035141) },
    { COEF_CONST(-0.65889132022858), COEF_CONST(0.58835631608963) },
    { COEF_CONST(0.98838084936142), COEF_CONST(0.99994349479675) },
    { COEF_CONST(-0.20651349425316), COEF_CONST(0.54593044519424) },
    { COEF_CONST(-0.62126415967941), COEF_CONST(-0.59893679618835) },
    { COEF_CONST(0.20320105552673), COEF_CONST(-0.86879181861877) },
    { COEF_CONST(-0.97790551185608), COEF_CONST(0.96290808916092) },
    { COEF_CONST(0.11112534999847), COEF_CONST(0.21484763920307) },
    { COEF_CONST(-0.41368338465691), COEF_CONST(0.28216838836670) },
    { COEF_CONST(0.24133038520813), COEF_CONST(0.51294362545013) },
    { COEF_CONST(-0.66393411159515), COEF_CONST(-0.08249679952860) },
    { COEF_CONST(-0.53697830438614), COEF_CONST(-0.97649902105331) },
    { COEF_CONST(-0.97224736213684), COEF_CONST(0.22081333398819) },
    { COEF_CONST(0.87392479181290), COEF_CONST(-0.12796173989773) },
    { COEF_CONST(0.19050361216068), COEF_CONST(0.01602615416050) },
    { COEF_CONST(-0.46353441476822), COEF_CONST(-0.95249038934708) },
    { COEF_CONST(-0.07064096629620), COEF_CONST(-0.94479805231094) },
    { COEF_CONST(-0.92444086074829), COEF_CONST(-0.10457590222359) },
    { COEF_CONST(-0.83822596073151), COEF_CONST(-0.01695043221116) },
    { COEF_CONST(0.75214684009552), COEF_CONST(-0.99955683946609) },
    { COEF_CONST(-0.42102998495102), COEF_CONST(0.99720942974091) },
    { COEF_CONST(-0.72094786167145), COEF_CONST(-0.35008960962296) },
    { COEF_CONST(0.78843313455582), COEF_CONST(0.52851396799088) },
    { COEF_CONST(0.97394025325775), COEF_CONST(-0.26695942878723) },
    { COEF_CONST(0.99206465482712), COEF_CONST(-0.57010120153427) },
    { COEF_CONST(0.76789611577988), COEF_CONST(-0.76519358158112) },
    { COEF_CONST(-0.82002419233322), COEF_CONST(-0.73530179262161) },
    { COEF_CONST(0.81924992799759), COEF_CONST(0.99698424339294) },
    { COEF_CONST(-0.26719850301743), COEF_CONST(0.68903368711472) },
    { COEF_CONST(-0.43311259150505), COEF_CONST(0.85321813821793) },
    { COEF_CONST(0.99194979667664), COEF_CONST(0.91876250505447) },
    { COEF_CONST(-0.80691999197006), COEF_CONST(-0.32627540826797) },
    { COEF_CONST(0.43080005049706), COEF_CONST(-0.21919095516205) },
    { COEF_CONST(0.67709493637085), COEF_CONST(-0.95478075742722) },
    { COEF_CONST(0.56151771545410), COEF_CONST(-0.70693808794022) },
    { COEF_CONST(0.10831862688065), COEF_CONST(-0.08628837019205) },
    { COEF_CONST(0.91229414939880), COEF_CONST(-0.65987348556519) },
    { COEF_CONST(-0.48972892761230), COEF_CONST(0.56289243698120) },
    { COEF_CONST(-0.89033657312393), COEF_CONST(-0.71656566858292) },
    { COEF_CONST(0.65269446372986), COEF_CONST(0.65916007757187) },
    { COEF_CONST(0.67439478635788), COEF_CONST(-0.81684380769730) },
    { COEF_CONST(-0.47770830988884), COEF_CONST(-0.16789555549622) },
    { COEF_CONST(-0.99715977907181), COEF_CONST(-0.93565785884857) },
    { COEF_CONST(-0.90889590978622), COEF_CONST(0.62034398317337) },
    { COEF_CONST(-0.06618622690439), COEF_CONST(-0.23812216520309) },
    { COEF_CONST(0.99430269002914), COEF_CONST(0.18812555074692) },
    { COEF_CONST(0.97686403989792), COEF_CONST(-0.28664535284042) },
    { COEF_CONST(0.94813650846481), COEF_CONST(-0.97506642341614) },
    { COEF_CONST(-0.95434498786926), COEF_CONST(-0.79607981443405) },
    { COEF_CONST(-0.49104782938957), COEF_CONST(0.32895213365555) },
    { COEF_CONST(0.99881172180176), COEF_CONST(0.88993984460831) },
    { COEF_CONST(0.50449168682098), COEF_CONST(-0.85995072126389) },
    { COEF_CONST(0.47162890434265), COEF_CONST(-0.18680204451084) },
    { COEF_CONST(-0.62081581354141), COEF_CONST(0.75000673532486) },
    { COEF_CONST(-0.43867015838623), COEF_CONST(0.99998068809509) },
    { COEF_CONST(0.98630565404892), COEF_CONST(-0.53578901290894) },
    { COEF_CONST(-0.61510360240936), COEF_CONST(-0.89515018463135) },
    { COEF_CONST(-0.03841517493129), COEF_CONST(-0.69888818264008) },
    { COEF_CONST(-0.30102157592773), COEF_CONST(-0.07667808979750) },
    { COEF_CONST(0.41881284117699), COEF_CONST(0.02188098989427) },
    { COEF_CONST(-0.86135452985764), COEF_CONST(0.98947483301163) },
    { COEF_CONST(0.67226862907410), COEF_CONST(-0.13494388759136) },
    { COEF_CONST(-0.70737397670746), COEF_CONST(-0.76547348499298) },
    { COEF_CONST(0.94044947624207), COEF_CONST(0.09026201069355) },
    { COEF_CONST(-0.82386350631714), COEF_CONST(0.08924768865108) },
    { COEF_CONST(-0.32070666551590), COEF_CONST(0.50143420696259) },
    { COEF_CONST(0.57593160867691), COEF_CONST(-0.98966425657272) },
    { COEF_CONST(-0.36326017975807), COEF_CONST(0.07440242916346) },
    { COEF_CONST(0.99979043006897), COEF_CONST(-0.14130286872387) },
    { COEF_CONST(-0.92366021871567), COEF_CONST(-0.97979295253754) },
    { COEF_CONST(-0.44607177376747), COEF_CONST(-0.54233253002167) },
    { COEF_CONST(0.44226801395416), COEF_CONST(0.71326756477356) },
    { COEF_CONST(0.03671907261014), COEF_CONST(0.63606387376785) },
    { COEF_CONST(0.52175426483154), COEF_CONST(-0.85396826267242) },
    { COEF_CONST(-0.94701141119003), COEF_CONST(-0.01826348155737) },
    { COEF_CONST(-0.98759609460831), COEF_CONST(0.82288712263107) },
    { COEF_CONST(0.87434792518616), COEF_CONST(0.89399492740631) },
    { COEF_CONST(-0.93412041664124), COEF_CONST(0.41374051570892) },
    { COEF_CONST(0.96063941717148), COEF_CONST(0.93116706609726) },
    { COEF_CONST(0.97534251213074), COEF_CONST(0.86150932312012) },
    { COEF_CONST(0.99642467498779), COEF_CONST(0.70190042257309) },
    { COEF_CONST(-0.94705086946487), COEF_CONST(-0.29580041766167) },
    { COEF_CONST(0.91599804162979), COEF_CONST(-0.98147833347321) }
};

#ifdef __cplusplus

#endif
#endif