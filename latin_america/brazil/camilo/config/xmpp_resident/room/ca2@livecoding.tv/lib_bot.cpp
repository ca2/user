
string on_bot(string strUser,string strText)
{
   m_strText = strText;
   m_strUser = strUser;
   m_strTopicUser = strUser;
   m_epersonVocative = ::vericard::person_user;
   m_epersonTopic = ::vericard::person_user;
   string strTopic;
   stringa t_straParam;
   string strSpeakText;

   {

      string strLang;

      sp(chat_item) pitem = Application.veripack().get_scheduled_speech(strUser, strText, strLang);

      if (pitem.is_set())
      {

         m_bSpeak = pitem->m_bSpeak;

         m_pcomm->add_chat(this);

         m_epersonTopic = pitem->m_epersonTopic;
         m_epersonVocative = pitem->m_epersonVocative;

         m_strUser = pitem->m_strUser;
         m_strName = pitem->m_strName;
         m_strLang = pitem->m_strLang;

         m_strOther = pitem->m_strOther;
         m_strOtherName = pitem->m_strOtherName;
         m_strOtherLang = pitem->m_strOtherLang;

         pitem->m_bSpeak = false;

         pitem.release();

         lspeak(strUser, strLang, strText);

         return "";

      }

   }
   
   if (!strText.begins_ci("!") && !strText.begins_ci("?") && !m_straSpecialCommand.contains_ci(strText))
   {
      
      m_pcomm->add_chat(this);
   
      return "";

   }

   m_strCountry = get_user_country_code(m_strUser);
   m_strLang = get_user_lang(m_strUser);
   m_strTimeZone = get_user_time_zone(strText);
   m_strName = username(strUser, m_strLang);
   if(strText.get_length() > 9 && ::str::begins(strText,"!setname "))
   {
      var str = strText.substr(9);
      if (consider_speakable_text(str))
      {
         set_user_data(strUser, "name", str);
         set_user_data(strUser, "name." + m_strLang, str);
      }
      else
      {
         str = _t("It is not considered reasonably speakable text.");
      }

      lspeak(strUser,m_strLang,str);
      return str;
   }
   else if(strText.get_length() > 5 && ::str::begins(strText,"!say "))
   {
      var str = strText.substr(5);
      if (!consider_speakable_text(strText))
      {
         str = _t("I am not going to say that phrase");
      }
      if (!lspeak(strUser, m_strLang, str))
      {
         return str;
      }
      return "";
   }
   else if(strText.get_length() > 6 && ::str::begins(strText,"!play "))
   {
      stringa a = ls_names(m_pcomm->get_base_path() / "audio/element");
      string str = strText.substr(6);
      str = str.trimmed();

      string strServer = System.url().get_server(str);
      strsize iFind1 = -1;
      if ((iFind1 = strServer.find_ci("spotify.")) == 0
         || (iFind1 =strServer.find_ci(".spotify.")) > 0)
      {

         strsize iFind = str.find_ci("/track/", iFind1);
         if (iFind > 0)
         {
            string strId = str.Mid(iFind + strlen("/track/"));
            str = "spotify:track:" + strId;
         }

      }
      if (::str::begins_ci(str, "spotify:track:"))
      {

#ifdef WINDOWS

         call_async("C:\\core\\time\\Win32\\basis\\app_veriwell_waven.exe", str, "C:\\core\\time\\Win32\\basis\\", SW_SHOW, false);

#else

         call_async("/xcore/stage/x86/app", "\"" + str + "\" : app=app-veriwell/waven build_number=basis version=basis locale=_std schema=_std ", "/xcore/stage/x86/", SW_SHOW, false);

#endif

      }
      else
      {

         bool found = false;
         int i = 0;
         for (; i < a.get_count() && !found; i++)
         {
            if (a[i].lower() == str.lower())
            {
               found = true;
            }
         }
         if (found)
         {
            play_sound(m_pcomm->get_base_path() / "audio/element"/ str + ".wav");
            return "";
         }
         else
         {
            if (m_strLang == "ru")
            {

               return "Звук \"" + str + "\" не найден. Используйте команду !playlist для получения списка доступных звуков";

            }
            else
            {

               return "\"" + str + "\" not found. Please, use !playlist command to list existing sounds.";

            }

         }
      }
   }
   stringa tokens;
   tokens.explode(" ",strText);
   if(tokens.get_count() == 1)
   {
      if(::str::begins(tokens.last(),"!@"))
      {
         strUser = tokens[0].substr(2);
         strText = "!";
      }
   }
   else
   {
      if(::str::begins(tokens.last(),"@"))
      {
         
         m_strOther = tokens.last().substr(1);
         
         m_strOtherName = username(m_strOther, m_strLang);

         string str = "";
         int i = 0;
         for(; i < tokens.get_upper_bound(); i++)
         {
            if(i > 0)
            {
               str += " ";
            }
            str += tokens[i];
         }
         strText = str;

         m_strOtherCountry = get_user_country_code(m_strOther);
         m_strOtherTimeZone = get_user_time_zone(m_strOther);
         if (m_strOtherTimeZone.is_empty())
         {

            if (m_strOtherCountry.has_char())
            {
               m_strOtherTimeZone = initial_locality_time_zone(m_strOtherCountry, get_lctv_info(m_strOther, "city"));
               set_user_data(m_strOther, "time_zone_text", m_strOtherTimeZone);
               set_user_data(m_strOther, "time_zone", time_zone(m_strOtherTimeZone));
            }
         }

      }
   }
   m_strText = strText;
   return bot_x();
}