package models

import akka.actor._
import scala.concurrent.duration._
import scala.concurrent.Future

import play.api._
import play.api.libs.json._
import play.api.libs.iteratee._
import play.api.libs.concurrent._

import akka.util.Timeout
import akka.pattern.ask

import play.api.Play.current
import play.api.libs.concurrent.Execution.Implicits._

import scala.collection.mutable

object ChatRoom {
  
  implicit val timeout = Timeout(1 second)
  
  lazy val default = Akka.system.actorOf(Props[ChatRoom])

  def join(username:String): Future[(Iteratee[JsValue,_],Enumerator[JsValue])] = {

    (default ? Join).map {  
      case Connected(enumerator) => 
        // Create an Iteratee to consume the feed
        val iteratee = Iteratee.foreach[JsValue] { event =>
         Logger("nicolas").info(event.toString)
        }

        (iteratee,enumerator) 
    }
  }
}

class ChatRoom extends Actor {
  
  val votes = new mutable.HashMap[String, mutable.Set[String]] with mutable.MultiMap[String, String]

  val loggerIteratee = Iteratee.foreach[JsValue](event => Logger("chatRoom").info(event.toString))
  val (likeEnumerator, likeChannel) = Concurrent.broadcast[JsValue]
  likeEnumerator |>> loggerIteratee

  def receive = {
    case Join => {
      sender ! Connected(likeEnumerator)
    }

    case Like(confId, nfcId) => {
      votes.addBinding(confId, nfcId)
      notifyNewStats()
    }    
  }
  
  def notifyNewStats() {
    val stats = votes.toVector.map({ case (k,s) => (k, s.size) }).sortWith(_._2 > _._2) 

    val msg = JsObject(Seq(
        "stats" -> Json.toJson( stats.map {case (k,n) => JsObject(Seq("confName"-> JsString(k), "nbLikes"->JsNumber(n))) } )
      ))
    likeChannel.push(msg)
  }
  
}

case class Join()
case class Like(confId: String, nfcId: String)

case class Connected(enumerator:Enumerator[JsValue])
