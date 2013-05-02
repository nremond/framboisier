package controllers

import play.api._
import play.api.mvc._

import play.api.libs.json._
import play.api.libs.iteratee._

import models._

import akka.actor._
import scala.concurrent.duration._

object Application extends Controller {
  
  /**
   * Just display the home page.
   */
  def index = Action { implicit request =>
    Ok(views.html.index())
  }
  
  
  /**
   * Handles the chat websocket.
   */
  def chat = WebSocket.async[JsValue] { request  =>    
    Leaderboard.join("FakeUsername")
  }

  def conf = Action { implicit request =>
    Ok(Json.arr("NFC on raspberry",
        "framboisier voting system",
        "nfc on android",
        "is javascript a functional language?",
        "Iteratee for noobz",
        "Agile bullshit",
        "Mobile bullshit",
        "nfc on android2",
        "is javascript a functional language?2",
        "Iteratee for noobz2"))
  }

//curl -X POST -d "nfcId=3117" http://localhost:9000/like/12


  import play.api.data._
  import play.api.data.Forms._

  case class NfcDevice(nfcId: String)

  val nfcForm = Form(mapping(
    "nfcId" -> nonEmptyText)(NfcDevice.apply)(NfcDevice.unapply))

  
  def like(confId :Int) = Action {
    implicit request => nfcForm.bindFromRequest.fold(
      formWithErrors => BadRequest("crotte"), //formWithErrors.toString),
      nfcDevice => {
        Leaderboard.default ! Like(confId.toString, nfcDevice.nfcId)
        Ok(s"c'est tout bon mon roger, nfcId=${nfcDevice.nfcId}, confId=$confId\n")
      }
    )
  }


}
